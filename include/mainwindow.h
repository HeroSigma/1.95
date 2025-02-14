#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QString>
#include <QModelIndex>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QGraphicsPixmapItem>
#include <QGraphicsItemGroup>
#include <QGraphicsSceneMouseEvent>
#include <QCloseEvent>
#include <QAbstractItemModel>
#include <QJSValue>
#include "project.h"
#include "orderedjson.h"
#include "config.h"
#include "map.h"
#include "editor.h"
#include "tileseteditor.h"
#include "regionmapeditor.h"
#include "mapimageexporter.h"
#include "filterchildrenproxymodel.h"
#include "maplistmodels.h"
#include "shortcutseditor.h"
#include "preferenceeditor.h"
#include "projectsettingseditor.h"
#include "gridsettings.h"
#include "customscriptseditor.h"
#include "wildmonchart.h"
#include "wildmonsearch.h"
#include "updatepromoter.h"
#include "aboutporymap.h"
#include "mapheaderform.h"
#include "newlayoutdialog.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent);
    ~MainWindow();

    MainWindow() = delete;
    MainWindow(const MainWindow &) = delete;
    MainWindow & operator = (const MainWindow &) = delete;

    // Scripting API
    Q_INVOKABLE QJSValue getBlock(int x, int y);
    void tryRedrawMapArea(bool forceRedraw);
    void tryCommitMapChanges(bool commitChanges);
    Q_INVOKABLE void setBlock(int x, int y, int metatileId, int collision, int elevation, bool forceRedraw = true, bool commitChanges = true);
    Q_INVOKABLE void setBlock(int x, int y, int rawValue, bool forceRedraw = true, bool commitChanges = true);
    Q_INVOKABLE void setBlocksFromSelection(int x, int y, bool forceRedraw = true, bool commitChanges = true);
    Q_INVOKABLE int getMetatileId(int x, int y);
    Q_INVOKABLE void setMetatileId(int x, int y, int metatileId, bool forceRedraw = true, bool commitChanges = true);
    Q_INVOKABLE int getBorderMetatileId(int x, int y);
    Q_INVOKABLE void setBorderMetatileId(int x, int y, int metatileId, bool forceRedraw = true, bool commitChanges = true);
    Q_INVOKABLE int getCollision(int x, int y);
    Q_INVOKABLE void setCollision(int x, int y, int collision, bool forceRedraw = true, bool commitChanges = true);
    Q_INVOKABLE int getElevation(int x, int y);
    Q_INVOKABLE void setElevation(int x, int y, int elevation, bool forceRedraw = true, bool commitChanges = true);
    Q_INVOKABLE void bucketFill(int x, int y, int metatileId, bool forceRedraw = true, bool commitChanges = true);
    Q_INVOKABLE void bucketFillFromSelection(int x, int y, bool forceRedraw = true, bool commitChanges = true);
    Q_INVOKABLE void magicFill(int x, int y, int metatileId, bool forceRedraw = true, bool commitChanges = true);
    Q_INVOKABLE void magicFillFromSelection(int x, int y, bool forceRedraw = true, bool commitChanges = true);
    Q_INVOKABLE void shift(int xDelta, int yDelta, bool forceRedraw = true, bool commitChanges = true);
    Q_INVOKABLE void redraw();
    Q_INVOKABLE void commit();
    Q_INVOKABLE QJSValue getDimensions();
    Q_INVOKABLE int getWidth();
    Q_INVOKABLE int getHeight();
    Q_INVOKABLE QJSValue getBorderDimensions();
    Q_INVOKABLE int getBorderWidth();
    Q_INVOKABLE int getBorderHeight();
    Q_INVOKABLE void setDimensions(int width, int height);
    Q_INVOKABLE void setWidth(int width);
    Q_INVOKABLE void setHeight(int height);
    Q_INVOKABLE void setBorderDimensions(int width, int height);
    Q_INVOKABLE void setBorderWidth(int width);
    Q_INVOKABLE void setBorderHeight(int height);
    void refreshAfterPaletteChange(Tileset *tileset);
    void setTilesetPalette(Tileset *tileset, int paletteIndex, QList<QList<int>> colors);
    Q_INVOKABLE void setPrimaryTilesetPalette(int paletteIndex, QList<QList<int>> colors, bool forceRedraw = true);
    Q_INVOKABLE void setPrimaryTilesetPalettes(QList<QList<QList<int>>> palettes, bool forceRedraw = true);
    Q_INVOKABLE void setSecondaryTilesetPalette(int paletteIndex, QList<QList<int>> colors, bool forceRedraw = true);
    Q_INVOKABLE void setSecondaryTilesetPalettes(QList<QList<QList<int>>> palettes, bool forceRedraw = true);
    QJSValue getTilesetPalette(const QList<QList<QRgb>> &palettes, int paletteIndex);
    QJSValue getTilesetPalettes(const QList<QList<QRgb>> &palettes);
    Q_INVOKABLE QJSValue getPrimaryTilesetPalette(int paletteIndex);
    Q_INVOKABLE QJSValue getPrimaryTilesetPalettes();
    Q_INVOKABLE QJSValue getSecondaryTilesetPalette(int paletteIndex);
    Q_INVOKABLE QJSValue getSecondaryTilesetPalettes();
    void refreshAfterPalettePreviewChange();
    void setTilesetPalettePreview(Tileset *tileset, int paletteIndex, QList<QList<int>> colors);
    Q_INVOKABLE void setPrimaryTilesetPalettePreview(int paletteIndex, QList<QList<int>> colors, bool forceRedraw = true);
    Q_INVOKABLE void setPrimaryTilesetPalettesPreview(QList<QList<QList<int>>> palettes, bool forceRedraw = true);
    Q_INVOKABLE void setSecondaryTilesetPalettePreview(int paletteIndex, QList<QList<int>> colors, bool forceRedraw = true);
    Q_INVOKABLE void setSecondaryTilesetPalettesPreview(QList<QList<QList<int>>> palettes, bool forceRedraw = true);
    Q_INVOKABLE QJSValue getPrimaryTilesetPalettePreview(int paletteIndex);
    Q_INVOKABLE QJSValue getPrimaryTilesetPalettesPreview();
    Q_INVOKABLE QJSValue getSecondaryTilesetPalettePreview(int paletteIndex);
    Q_INVOKABLE QJSValue getSecondaryTilesetPalettesPreview();
    Q_INVOKABLE int getNumPrimaryTilesetMetatiles();
    Q_INVOKABLE int getNumSecondaryTilesetMetatiles();
    Q_INVOKABLE int getNumPrimaryTilesetTiles();
    Q_INVOKABLE int getNumSecondaryTilesetTiles();
    Q_INVOKABLE QString getPrimaryTileset();
    Q_INVOKABLE QString getSecondaryTileset();
    Q_INVOKABLE void setPrimaryTileset(QString tileset);
    Q_INVOKABLE void setSecondaryTileset(QString tileset);
    void saveMetatilesByMetatileId(int metatileId);
    void saveMetatileAttributesByMetatileId(int metatileId);
    Metatile * getMetatile(int metatileId);
    Q_INVOKABLE QString getMetatileLabel(int metatileId);
    Q_INVOKABLE void setMetatileLabel(int metatileId, QString label);
    Q_INVOKABLE int getMetatileLayerType(int metatileId);
    Q_INVOKABLE void setMetatileLayerType(int metatileId, int layerType);
    Q_INVOKABLE int getMetatileEncounterType(int metatileId);
    Q_INVOKABLE void setMetatileEncounterType(int metatileId, int encounterType);
    Q_INVOKABLE int getMetatileTerrainType(int metatileId);
    Q_INVOKABLE void setMetatileTerrainType(int metatileId, int terrainType);
    Q_INVOKABLE int getMetatileBehavior(int metatileId);
    Q_INVOKABLE void setMetatileBehavior(int metatileId, int behavior);
    Q_INVOKABLE QString getMetatileBehaviorName(int metatileId);
    Q_INVOKABLE void setMetatileBehaviorName(int metatileId, QString behavior);
    Q_INVOKABLE int getMetatileAttributes(int metatileId);
    Q_INVOKABLE void setMetatileAttributes(int metatileId, int attributes);
    Q_INVOKABLE QJSValue getMetatileTile(int metatileId, int tileIndex);
    Q_INVOKABLE void setMetatileTile(int metatileId, int tileIndex, int tileId, bool xflip, bool yflip, int palette, bool forceRedraw = true);
    Q_INVOKABLE void setMetatileTile(int metatileId, int tileIndex, QJSValue tileObj, bool forceRedraw = true);
    int calculateTileBounds(int * tileStart, int * tileEnd);
    Q_INVOKABLE QJSValue getMetatileTiles(int metatileId, int tileStart = 0, int tileEnd = -1);
    Q_INVOKABLE void setMetatileTiles(int metatileId, QJSValue tilesObj, int tileStart = 0, int tileEnd = -1, bool forceRedraw = true);
    Q_INVOKABLE void setMetatileTiles(int metatileId, int tileId, bool xflip, bool yflip, int palette, int tileStart = 0, int tileEnd = -1, bool forceRedraw = true);
    Q_INVOKABLE QJSValue getTilePixels(int tileId);
    Q_INVOKABLE QString getSong();
    Q_INVOKABLE void setSong(QString song);
    Q_INVOKABLE QString getLocation();
    Q_INVOKABLE void setLocation(QString location);
    Q_INVOKABLE bool getRequiresFlash();
    Q_INVOKABLE void setRequiresFlash(bool require);
    Q_INVOKABLE QString getWeather();
    Q_INVOKABLE void setWeather(QString weather);
    Q_INVOKABLE QString getType();
    Q_INVOKABLE void setType(QString type);
    Q_INVOKABLE QString getBattleScene();
    Q_INVOKABLE void setBattleScene(QString battleScene);
    Q_INVOKABLE bool getShowLocationName();
    Q_INVOKABLE void setShowLocationName(bool show);
    Q_INVOKABLE bool getAllowRunning();
    Q_INVOKABLE void setAllowRunning(bool allow);
    Q_INVOKABLE bool getAllowBiking();
    Q_INVOKABLE void setAllowBiking(bool allow);
    Q_INVOKABLE bool getAllowEscaping();
    Q_INVOKABLE void setAllowEscaping(bool allow);
    Q_INVOKABLE int getFloorNumber();
    Q_INVOKABLE void setFloorNumber(int floorNumber);

public slots:
    void on_mainTabBar_tabBarClicked(int index);
    void on_mapViewTab_tabBarClicked(int index);
    void onWarpBehaviorWarningClicked();
    void clearOverlay();

private slots:
    void on_action_Open_Project_triggered();
    void on_action_Reload_Project_triggered();
    void on_action_Close_Project_triggered();
    void on_action_Save_Project_triggered();

    void openWarpMap(QString map_name, int event_id, Event::Group event_group);

    void duplicate();
    void setClipboardData(poryjson::Json::object);
    void setClipboardData(QImage);
    void setClipboardData(const QString &text);
    void copy();
    void paste();

    void onLayoutChanged(Layout *layout);
    void onOpenConnectedMap(MapConnection*);
    void onTilesetsSaved(QString, QString);
    void onNewMapCreated(Map *newMap, const QString &groupName);
    void onNewMapGroupCreated(const QString &groupName);
    void onNewMapSectionCreated(const QString &idName);
    void onMapSectionDisplayNameChanged(const QString &idName, const QString &displayName);
    void onNewLayoutCreated(Layout *layout);
    void onNewTilesetCreated(Tileset *tileset);
    void onMapLoaded(Map *map);
    void onMapRulerStatusChanged(const QString &);
    void applyUserShortcuts();
    void markMapEdited();
    void markSpecificMapEdited(Map*);

    void on_actionNew_Tileset_triggered();
    void on_action_Save_triggered();
    void on_action_Exit_triggered();
    void on_comboBox_LayoutSelector_currentTextChanged(const QString &text);
    void on_actionShortcuts_triggered();

    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();
    void on_actionBetter_Cursors_triggered();
    void on_actionPlayer_View_Rectangle_triggered();
    void on_actionCursor_Tile_Outline_triggered();
    void on_actionPencil_triggered();
    void on_actionPointer_triggered();
    void on_actionFlood_Fill_triggered();
    void on_actionEyedropper_triggered();
    void on_actionMove_triggered();
    void on_actionMap_Shift_triggered();

    void addNewEvent(Event::Type type);
    void tryAddEventTab(QWidget * tab);
    void displayEventTabs();
    void updateSelectedEvents();
    void updateEvents();

    void on_toolButton_Paint_clicked();
    void on_toolButton_Select_clicked();
    void on_toolButton_Fill_clicked();
    void on_toolButton_Dropper_clicked();
    void on_toolButton_Move_clicked();
    void on_toolButton_Shift_clicked();

    void onOpenMapListContextMenu(const QPoint &point);
    void currentMetatilesSelectionChanged();

    void on_action_Export_Map_Image_triggered();
    void on_actionExport_Stitched_Map_Image_triggered();
    void on_actionExport_Map_Timelapse_Image_triggered();
    void on_actionImport_Map_from_Advance_Map_1_92_triggered();

    void on_pushButton_AddConnection_clicked();
    void on_button_OpenDiveMap_clicked();
    void on_button_OpenEmergeMap_clicked();
    void on_comboBox_DiveMap_currentTextChanged(const QString &mapName);
    void on_comboBox_EmergeMap_currentTextChanged(const QString &mapName);
    void on_comboBox_PrimaryTileset_currentTextChanged(const QString &arg1);
    void on_comboBox_SecondaryTileset_currentTextChanged(const QString &arg1);
    void on_pushButton_ChangeDimensions_clicked();
    void on_checkBox_smartPaths_stateChanged(int selected);
    void on_checkBox_ToggleBorder_stateChanged(int selected);

    void resetMapViewScale();

    void on_actionTileset_Editor_triggered();

    void moveEvent(QMoveEvent *event);
    void closeEvent(QCloseEvent *);

    void eventTabChanged(int index);

    void on_checkBox_MirrorConnections_stateChanged(int selected);
    void on_actionDive_Emerge_Map_triggered();
    void on_actionShow_Events_In_Map_View_triggered();
    void on_groupBox_DiveMapOpacity_toggled(bool on);
    void on_slider_DiveEmergeMapOpacity_valueChanged(int value);
    void on_slider_DiveMapOpacity_valueChanged(int value);
    void on_slider_EmergeMapOpacity_valueChanged(int value);
    void on_horizontalSlider_CollisionTransparency_valueChanged(int value);

    void mapListShortcut_ToggleEmptyFolders();
    void mapListShortcut_ExpandAll();
    void mapListShortcut_CollapseAll();

    void on_actionAbout_Porymap_triggered();
    void on_actionOpen_Log_File_triggered();
    void on_actionOpen_Config_Folder_triggered();
    void on_horizontalSlider_MetatileZoom_valueChanged(int value);
    void on_horizontalSlider_CollisionZoom_valueChanged(int value);
    void on_pushButton_NewWildMonGroup_clicked();
    void on_pushButton_DeleteWildMonGroup_clicked();
    void on_pushButton_SummaryChart_clicked();
    void on_pushButton_ConfigureEncountersJSON_clicked();
    void on_toolButton_WildMonSearch_clicked();
    void on_pushButton_CreatePrefab_clicked();
    void on_spinBox_SelectedElevation_valueChanged(int elevation);
    void on_spinBox_SelectedCollision_valueChanged(int collision);
    void on_actionRegion_Map_Editor_triggered();
    void on_actionPreferences_triggered();
    void on_actionCheck_for_Updates_triggered();
    void togglePreferenceSpecificUi();
    void on_actionProject_Settings_triggered();
    void on_actionCustom_Scripts_triggered();
    void reloadScriptEngine();
    void on_actionShow_Grid_triggered();
    void on_actionGrid_Settings_triggered();
    void openWildMonTable(const QString &mapName, const QString &groupName, const QString &fieldName);

public:
    Ui::MainWindow *ui;
    QPointer<Editor> editor = nullptr;

private:
    QLabel *label_MapRulerStatus = nullptr;
    QPointer<TilesetEditor> tilesetEditor = nullptr;
    QPointer<RegionMapEditor> regionMapEditor = nullptr;
    QPointer<ShortcutsEditor> shortcutsEditor = nullptr;
    QPointer<MapImageExporter> mapImageExporter = nullptr;
    QPointer<PreferenceEditor> preferenceEditor = nullptr;
    QPointer<ProjectSettingsEditor> projectSettingsEditor = nullptr;
    QPointer<GridSettingsDialog> gridSettingsDialog = nullptr;
    QPointer<CustomScriptsEditor> customScriptsEditor = nullptr;

    QPointer<FilterChildrenProxyModel> groupListProxyModel = nullptr;
    QPointer<MapGroupModel> mapGroupModel = nullptr;
    QPointer<FilterChildrenProxyModel> locationListProxyModel = nullptr;
    QPointer<MapLocationModel> mapLocationModel = nullptr;
    QPointer<FilterChildrenProxyModel> layoutListProxyModel = nullptr;
    QPointer<LayoutTreeModel> layoutTreeModel = nullptr;

    QPointer<UpdatePromoter> updatePromoter = nullptr;
    QPointer<NetworkAccessManager> networkAccessManager = nullptr;
    QPointer<AboutPorymap> aboutWindow = nullptr;
    QPointer<WildMonChart> wildMonChart = nullptr;
    QPointer<WildMonSearch> wildMonSearch = nullptr;

    QAction *undoAction = nullptr;
    QAction *redoAction = nullptr;
    QPointer<QUndoView> undoView = nullptr;

    QAction *copyAction = nullptr;
    QAction *pasteAction = nullptr;

    MapHeaderForm *mapHeaderForm = nullptr;

    QMap<Event::Group, DraggablePixmapItem*> lastSelectedEvent;

    bool isProgrammaticEventTabChange;

    bool tilesetNeedsRedraw = false;

    bool setLayout(QString layoutId);
    bool setMap(QString);
    void unsetMap();
    bool userSetLayout(QString layoutId);
    bool userSetMap(QString);
    void redrawMapScene();
    void refreshMapScene();
    void refreshMetatileViews();
    void refreshCollisionSelector();
    void setLayoutOnlyMode(bool layoutOnly);

    bool checkProjectSanity();
    bool loadProjectData();
    bool setProjectUI();
    void clearProjectUI();

    void openEditHistory();
    void openNewMapDialog();
    void openDuplicateMapDialog(const QString &mapName);
    NewLayoutDialog* createNewLayoutDialog(const Layout *layoutToCopy = nullptr);
    void openNewLayoutDialog();
    void openDuplicateLayoutDialog(const QString &layoutId);
    void openNewMapGroupDialog();
    void openNewLocationDialog();
    void openSubWindow(QWidget * window);
    void scrollMapList(MapTree *list, const QString &itemName);
    void scrollMapListToCurrentMap(MapTree *list);
    void scrollMapListToCurrentLayout(MapTree *list);
    void resetMapListFilters();
    void showFileWatcherWarning(QString filepath);
    QString getExistingDirectory(QString);
    bool openProject(QString dir, bool initial = false);
    bool closeProject();
    void showRecentError(const QString &baseMessage);
    void showProjectOpenFailure();
    void showMapsExcludedAlert(const QStringList &excludedMapNames);

    bool setInitialMap();
    void saveGlobalConfigs();

    void refreshRecentProjectsMenu();

    void updateMapList();
    void openMapListItem(const QModelIndex &index);
    void onMapListTabChanged(int index);

    void displayMapProperties();
    void checkToolButtons();
    void clickToolButtonFromEditAction(Editor::EditAction editAction);

    void updateWindowTitle();

    void initWindow();
    void initCustomUI();
    void initExtraSignals();
    void initEditor();
    void initMiscHeapObjects();
    void initMapList();
    void initShortcuts();
    void initExtraShortcuts();
    void loadUserSettings();
    void restoreWindowState();
    void setTheme(QString);
    void updateTilesetEditor();
    Event::Group getEventGroupFromTabWidget(QWidget *tab);
    bool closeSupplementaryWindows();
    void setWindowDisabled(bool);
    void resetMapCustomAttributesTable();
    void initTilesetEditor();
    bool initRegionMapEditor(bool silent = false);
    bool askToFixRegionMapEditor();
    void initShortcutsEditor();
    void initCustomScriptsEditor();
    void connectSubEditorsToShortcutsEditor();
    void openProjectSettingsEditor(int tab);
    bool isProjectOpen();
    void showExportMapImageWindow(ImageExporterMode mode);
    double getMetatilesZoomScale();
    void redrawMetatileSelection();
    void scrollMetatileSelectorToSelection();
    MapListToolBar* getCurrentMapListToolBar();
    MapTree* getCurrentMapList();
    void setLocationComboBoxes(const QStringList &locations);

    QObjectList shortcutableObjects() const;
    void addCustomHeaderValue(QString key, QJsonValue value, bool isNew = false);

    void checkForUpdates(bool requestedByUser);
    void setDivingMapsVisible(bool visible);
};

// These are namespaced in a struct to avoid colliding with e.g. class Map.
struct MainTab {
    enum {
        Map,
        Events,
        Header,
        Connections,
        WildPokemon,
    };
};

struct MapViewTab {
    enum {
        Metatiles,
        Collision,
        Prefabs,
    };
};

struct MapListTab {
    enum {
        Groups = 0, Locations, Layouts
    };
};

#endif // MAINWINDOW_H
