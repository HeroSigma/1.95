#include "advancemapparser.h"
#include "log.h"
#include "project.h"
#include "maplayout.h"

Layout *AdvanceMapParser::parseLayout(const QString &filepath, bool *error, const Project *project)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        *error = true;
        logError(QString("Could not open Advance Map 1.92 Map .map file '%1': ").arg(filepath) + file.errorString());
        return nullptr;
    }

    QByteArray in = file.readAll();
    file.close();

    if (in.length() < 20 || in.length() % 2 != 0) {
        *error = true;
        logError(QString("Advance Map 1.92 Map .map file '%1' is an unexpected size.").arg(filepath));
        return nullptr;
    }

    // Border width/height are 0 in RSE .map files.
    int borderWidth = static_cast<unsigned char>(in.at(16));
    int borderHeight = static_cast<unsigned char>(in.at(17));
    int numBorderTiles = borderWidth * borderHeight; // 0 if RSE

    int mapDataOffset = 20 + (numBorderTiles * 2); // FRLG .map files store border metatile data after the header
    int mapWidth = static_cast<unsigned char>(in.at(0)) |
                   (static_cast<unsigned char>(in.at(1)) << 8) |
                   (static_cast<unsigned char>(in.at(2)) << 16) |
                   (static_cast<unsigned char>(in.at(3)) << 24);
    int mapHeight = static_cast<unsigned char>(in.at(4)) |
                    (static_cast<unsigned char>(in.at(5)) << 8) |
                    (static_cast<unsigned char>(in.at(6)) << 16) |
                    (static_cast<unsigned char>(in.at(7)) << 24);
    int mapPrimaryTilesetNum = static_cast<unsigned char>(in.at(8)) |
                               (static_cast<unsigned char>(in.at(9)) << 8) |
                               (static_cast<unsigned char>(in.at(10)) << 16) |
                               (static_cast<unsigned char>(in.at(11)) << 24);
    int mapSecondaryTilesetNum = static_cast<unsigned char>(in.at(12)) |
                                 (static_cast<unsigned char>(in.at(13)) << 8) |
                                 (static_cast<unsigned char>(in.at(14)) << 16) |
                                 (static_cast<unsigned char>(in.at(15)) << 24);

    int numMetatiles = mapWidth * mapHeight;
    int baseMapSize = numMetatiles * 2;
    int baseBorderSize = numBorderTiles * 2;
    // Ensure the file has at least enough data for the header and one map.
    int mapDataEnd = mapDataOffset + baseMapSize;
    if (in.length() < mapDataEnd) {
        *error = true;
        logError(QString(".map file has too little data. Expected at least %1 bytes, but it has %2 bytes.")
                    .arg(mapDataEnd).arg(in.length()));
        return nullptr;
    }

    bool doubleMap = false;

    // Handle the RSE format where border data is stored at the end of the file.
    int rseBorderOffset = -1;
    if (numBorderTiles == 0) {
        // Map data follows the header, and border data (if present) is at the end
        // with width/height stored in the last 4 bytes.
        if (in.length() >= mapDataEnd + 4) {
            int borderWidthLE = static_cast<unsigned char>(in.at(in.length() - 4)) |
                                (static_cast<unsigned char>(in.at(in.length() - 3)) << 8);
            int borderHeightLE = static_cast<unsigned char>(in.at(in.length() - 2)) |
                                 (static_cast<unsigned char>(in.at(in.length() - 1)) << 8);
            int rseNumBorderTiles = borderWidthLE * borderHeightLE;
            int rseBorderSize = rseNumBorderTiles * 2;
            int borderOffset = in.length() - (rseBorderSize + 4);
            if (borderOffset >= mapDataOffset + baseMapSize) {
                // At least one full map and border data exist.
                if (borderOffset >= mapDataOffset + baseMapSize * 2) {
                    doubleMap = true; // Ignore the second layout; use the first.
                }
                borderWidth = borderWidthLE;
                borderHeight = borderHeightLE;
                numBorderTiles = rseNumBorderTiles;
                baseBorderSize = rseBorderSize;
                rseBorderOffset = borderOffset;
            }
        }
    } else {
        // FRLG format where border data comes right after the header.
        if (in.length() >= mapDataOffset + baseMapSize * 2) {
            doubleMap = true; // There is a second layout, ignore it.
        }
    }

    Blockdata blockdata;
    for (int i = mapDataOffset; (i + 1) < mapDataEnd && (i + 1) < in.length(); i += 2) {
        uint16_t word = static_cast<uint16_t>((in[i] & 0xff) + ((in[i + 1] & 0xff) << 8));
        blockdata.append(word);
    }

    Blockdata border;
    if (numBorderTiles != 0) {
        int borderOffset;
        if (mapDataOffset == 20) {
            // Border is at the end of the file after the map data (RSE).
            if (rseBorderOffset >= 0) {
                borderOffset = rseBorderOffset;
            } else {
                borderOffset = mapDataOffset + baseMapSize * (doubleMap ? 2 : 1);
            }
        } else {
            // Border data is directly after the header (FRLG).
            borderOffset = 20;
        }
        int borderEnd = borderOffset + baseBorderSize;
        for (int i = borderOffset; (i + 1) < borderEnd && (i + 1) < in.length(); i += 2) {
            uint16_t word = static_cast<uint16_t>((in[i] & 0xff) + ((in[i + 1] & 0xff) << 8));
            border.append(word);
        }
    }

    Layout *mapLayout = new Layout();
    mapLayout->width = mapWidth;
    mapLayout->height = mapHeight;
    mapLayout->border_width = (borderWidth == 0) ?  DEFAULT_BORDER_WIDTH : borderWidth;
    mapLayout->border_height = (borderHeight == 0) ?  DEFAULT_BORDER_HEIGHT : borderHeight;

    const QList<QString> tilesets = project->tilesetLabelsOrdered;

    const QString defaultPrimaryTileset = project->getDefaultPrimaryTilesetLabel();
    QString primaryTilesetLabel = tilesets.value(mapPrimaryTilesetNum, defaultPrimaryTileset);
    if (!project->primaryTilesetLabels.contains(primaryTilesetLabel)) {
        // AdvanceMap's primary tileset value points to a secondary tileset. Ignore it.
        primaryTilesetLabel = defaultPrimaryTileset;
    }
    const QString defaultSecondaryTileset = project->getDefaultSecondaryTilesetLabel();
    QString secondaryTilesetLabel = tilesets.value(mapSecondaryTilesetNum, defaultSecondaryTileset);
    if (!project->secondaryTilesetLabels.contains(secondaryTilesetLabel)) {
        // AdvanceMap's secondary tileset value points to a primary tileset. Ignore it.
        secondaryTilesetLabel = defaultSecondaryTileset;
    }

    mapLayout->tileset_primary_label = primaryTilesetLabel;
    mapLayout->tileset_secondary_label = secondaryTilesetLabel;

    mapLayout->blockdata = blockdata;

    if (!border.isEmpty()) {
        mapLayout->border = border;
    }

    return mapLayout;
}

QList<Metatile*> AdvanceMapParser::parseMetatiles(const QString &filepath, bool *error, bool primaryTileset)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        *error = true;
        logError(QString("Could not open Advance Map 1.92 Metatile .bvd file '%1': ").arg(filepath) + file.errorString());
        return { };
    }

    QByteArray in = file.readAll();
    file.close();

    if (in.length() < 9 || in.length() % 2 != 0) {
        *error = true;
        logError(QString("Advance Map 1.92 Metatile .bvd file '%1' is an unexpected size.").arg(filepath));
        return { };
    }

    int projIdOffset = in.length() - 4;
    int metatileSize = 16;
    BaseGameVersion version;
    if (in.at(projIdOffset + 0) == 'R'
     && in.at(projIdOffset + 1) == 'S'
     && in.at(projIdOffset + 2) == 'E'
     && in.at(projIdOffset + 3) == ' ') {
        // ruby and emerald are handled equally here.
        version = BaseGameVersion::pokeemerald;
    } else if (in.at(projIdOffset + 0) == 'F'
            && in.at(projIdOffset + 1) == 'R'
            && in.at(projIdOffset + 2) == 'L'
            && in.at(projIdOffset + 3) == 'G') {
        version = BaseGameVersion::pokefirered;
    } else {
        *error = true;
        logError(QString("Detected unsupported game type from .bvd file. Last 4 bytes of file must be 'RSE ' or 'FRLG'."));
        return { };
    }

    int attrSize = Metatile::getDefaultAttributesSize(version);
    int maxMetatiles = primaryTileset ? Project::getNumMetatilesPrimary() : Project::getNumMetatilesSecondary();
    int numMetatiles = static_cast<unsigned char>(in.at(0)) |
                                (static_cast<unsigned char>(in.at(1)) << 8) |
                                (static_cast<unsigned char>(in.at(2)) << 16) |
                                (static_cast<unsigned char>(in.at(3)) << 24);
    if (numMetatiles > maxMetatiles) {
        *error = true;
        logError(QString(".bvd file contains data for %1 metatiles, but the maximum number of metatiles is %2.").arg(numMetatiles).arg(maxMetatiles));
        return { };
    }
    if (numMetatiles < 1) {
        *error = true;
        logError(QString(".bvd file contains no data for metatiles."));
        return { };
    }

    int baseMetatileSize = metatileSize * numMetatiles;
    int baseAttrSize = attrSize * numMetatiles;
    int expectedSingleSize = baseMetatileSize + baseAttrSize + 8;
    int expectedDoubleSize = (baseMetatileSize * 2) + (baseAttrSize * 2) + 8;
    bool doubleTileset = false;
    if (in.length() == expectedDoubleSize) {
        doubleTileset = true;
    } else if (in.length() != expectedSingleSize) {
        *error = true;
        logError(QString(".bvd file is an unexpected size. Expected %1 or %2 bytes, but it has %3 bytes.").arg(expectedSingleSize).arg(expectedDoubleSize).arg(in.length()));
        return { };
    }

    int tilesOffset = 4;
    int attrsOffset;
    if (doubleTileset) {
        if (primaryTileset) {
            attrsOffset = 4 + (baseMetatileSize * 2);
        } else {
            tilesOffset += baseMetatileSize;
            attrsOffset = 4 + (baseMetatileSize * 2) + baseAttrSize;
        }
    } else {
        attrsOffset = 4 + baseMetatileSize;
    }

    QList<Metatile*> metatiles;
    for (int i = 0; i < numMetatiles; i++) {
        Metatile *metatile = new Metatile();
        QList<Tile> tiles;
        for (int j = 0; j < 8; j++) {
            int metatileOffset = tilesOffset + i * metatileSize + j * 2;
            Tile tile(static_cast<uint16_t>(
                        static_cast<unsigned char>(in.at(metatileOffset)) |
                       (static_cast<unsigned char>(in.at(metatileOffset + 1)) << 8)));
            tiles.append(tile);
        }

        // AdvanceMap .bvd files only contain 8 tiles of data per metatile.
        // If the user has triple-layer metatiles enabled we need to fill the remaining 4 tiles ourselves.
        if (projectConfig.tripleLayerMetatilesEnabled) {
            Tile tile = Tile();
            for (int j = 0; j < 4; j++)
                tiles.append(tile);
        }

        int attrOffset = attrsOffset + (i * attrSize);
        uint32_t attributes = 0;
        for (int j = 0; j < attrSize; j++)
            attributes |= static_cast<unsigned char>(in.at(attrOffset + j)) << (8 * j);
        metatile->setAttributes(attributes, version);
        metatile->tiles = tiles;
        metatiles.append(metatile);
    }

    return metatiles;
}

QList<QRgb> AdvanceMapParser::parsePalette(const QString &filepath, bool *error) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        *error = true;
        logError(QString("Could not open Advance Map 1.92 palette file '%1': ").arg(filepath) + file.errorString());
        return QList<QRgb>();
    }

    QByteArray in = file.readAll();
    file.close();

    if (in.length() % 4 != 0) {
        *error = true;
        logError(QString("Advance Map 1.92 palette file '%1' had an unexpected format. File's length must be a multiple of 4, but the length is %2.").arg(filepath).arg(in.length()));
        return QList<QRgb>();
    }

    QList<QRgb> palette;
    int i = 0;
    while (i < in.length()) {
        unsigned char red   = static_cast<unsigned char>(in.at(i + 0));
        unsigned char green = static_cast<unsigned char>(in.at(i + 1));
        unsigned char blue  = static_cast<unsigned char>(in.at(i + 2));
        palette.append(qRgb(red, green, blue));
        i += 4;
    }

    return palette;
}
