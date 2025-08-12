#pragma once
#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <QStringList>
#include "scriptutility.h"

class Block;
class Tile;
class MainWindow;

#if __has_include(<QJSEngine>)
#include <QJSEngine>
#endif


#ifdef QT_QML_LIB

// !! New callback functions or changes to existing callback function names/arguments
//    should be synced to resources/text/script_template.txt and docsrc/manual/scripting-capabilities.rst
enum CallbackType {
    OnProjectOpened,
    OnProjectClosed,
    OnBlockChanged,
    OnBorderMetatileChanged,
    OnBlockHoverChanged,
    OnBlockHoverCleared,
    OnMapOpened,
    OnLayoutOpened,
    OnMapResized,
    OnBorderResized,
    OnMapShifted,
    OnTilesetUpdated,
    OnMainTabChanged,
    OnMapViewTabChanged,
    OnBorderVisibilityToggled,
};

class Scripting
{
public:
    Scripting(MainWindow *mainWindow);
    ~Scripting();
    static void init(MainWindow *mainWindow);
    static void stop();
    static void populateGlobalObject(MainWindow *mainWindow);
    static QJSEngine *getEngine();
    static void invokeAction(int actionIndex);

    static void cb_ProjectOpened(QString projectPath);
    static void cb_ProjectClosed(QString projectPath);
    static void cb_MetatileChanged(int x, int y, Block prevBlock, Block newBlock);
    static void cb_BorderMetatileChanged(int x, int y, uint16_t prevMetatileId, uint16_t newMetatileId);
    static void cb_BlockHoverChanged(int x, int y);
    static void cb_BlockHoverCleared();
    static void cb_MapOpened(QString mapName);
    static void cb_LayoutOpened(QString layoutName);
    static void cb_MapResized(int oldWidth, int oldHeight, const QMargins &delta);
    static void cb_BorderResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
    static void cb_MapShifted(int xDelta, int yDelta);
    static void cb_TilesetUpdated(const QString &tilesetName);
    static void cb_MainTabChanged(int oldTab, int newTab);
    static void cb_MapViewTabChanged(int oldTab, int newTab);
    static void cb_BorderVisibilityToggled(bool visible);

    static bool tryErrorJS(QJSValue js);
    static QJSValue fromBlock(Block block);
    static QJSValue fromTile(Tile tile);
    static Tile toTile(QJSValue obj);
    static QJSValue dimensions(int width, int height);
    static QJSValue margins(const QMargins &margins);
    static QJSValue position(int x, int y);
    static const QImage * getImage(const QString &filepath, bool useCache);
    static QJSValue dialogInput(QJSValue input, bool selectedOk);

private:
    MainWindow *mainWindow;
    QJSEngine *engine;
    QStringList filepaths;
    QList<QJSValue> modules;
    QMap<QString, const QImage*> imageCache;
    ScriptUtility *scriptUtility;

    void loadModules(const QStringList &moduleFiles);
    void invokeCallback(CallbackType type, QJSValueList args);
};

#else

class Scripting
{
public:
    Scripting(MainWindow *) {}
    ~Scripting() {}
    static void init(MainWindow *) {}
    static void stop() {}
    static void populateGlobalObject(MainWindow *) {}

    static void cb_ProjectOpened(QString) {};
    static void cb_ProjectClosed(QString) {};
    static void cb_MetatileChanged(int, int, Block, Block) {};
    static void cb_BorderMetatileChanged(int, int, uint16_t, uint16_t) {};
    static void cb_BlockHoverChanged(int, int) {};
    static void cb_BlockHoverCleared() {};
    static void cb_MapOpened(QString) {};
    static void cb_LayoutOpened(QString) {};
    static void cb_MapResized(int, int, const QMargins &) {};
    static void cb_BorderResized(int, int, int, int) {};
    static void cb_MapShifted(int, int) {};
    static void cb_TilesetUpdated(const QString &) {};
    static void cb_MainTabChanged(int, int) {};
    static void cb_MapViewTabChanged(int, int) {};
    static void cb_BorderVisibilityToggled(bool) {};
};

#endif // QT_QML_LIB

#endif // SCRIPTING_H
