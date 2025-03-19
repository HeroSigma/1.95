#pragma once
#ifndef EDITOR_H
#define EDITOR_H

#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItemAnimation>
#include <QComboBox>
#include <QCheckBox>
#include <QCursor>
#include <QUndoGroup>
#include <QPointer>

#include "mapconnection.h"
#include "metatileselector.h"
#include "movementpermissionsselector.h"
#include "project.h"
#include "ui_mainwindow.h"
#include "bordermetatilespixmapitem.h"
#include "connectionpixmapitem.h"
#include "divingmappixmapitem.h"
#include "currentselectedmetatilespixmapitem.h"
#include "collisionpixmapitem.h"
#include "layoutpixmapitem.h"
#include "settings.h"
#include "gridsettings.h"
#include "movablerect.h"
#include "cursortilerect.h"
#include "mapruler.h"
#include "encountertablemodel.h"

class DraggablePixmapItem;
class MetatilesPixmapItem;

class Editor : public QObject
{
    Q_OBJECT
public:
    Editor(Ui::MainWindow* ui);
    ~Editor();

    Editor() = delete;
    Editor(const Editor &) = delete;
    Editor & operator = (const Editor &) = delete;

public:
    Ui::MainWindow* ui;

    QPointer<Project> project = nullptr;
    QPointer<Map> map = nullptr;
    QPointer<Layout> layout = nullptr;

    QUndoGroup editGroup; // Manages the undo history for each map

    Settings *settings;
    GridSettings gridSettings;

    void setProject(Project * project);
    void saveAll();
    void saveCurrent();
    void saveEncounterTabData();

    void closeProject();

    bool setMap(QString map_name);
    bool setLayout(QString layoutName);
    void unsetMap();

    Tileset *getCurrentMapPrimaryTileset();

    bool displayMap();
    bool displayLayout();

    void displayMetatileSelector();
    void displayMapMetatiles();
    void displayMapMovementPermissions();
    void displayBorderMetatiles();
    void displayCurrentMetatilesSelection();
    void redrawCurrentMetatilesSelection();
    void displayMovementPermissionSelector();
    void displayMapEvents();
    void displayMapConnections();
    void displayMapBorder();
    void displayMapGrid();
    void updateMapGrid();
    void displayWildMonTables();

    void updateMapBorder();
    void updateMapConnections();

    void setConnectionsVisibility(bool visible);
    void updateDivingMapsVisibility();
    void renderDivingConnections();
    void addConnection(MapConnection* connection);
    void removeConnection(MapConnection* connection);
    void addNewWildMonGroup(QWidget *window);
    void deleteWildMonGroup();
    void configureEncounterJSON(QWidget *);
    EncounterTableModel* getCurrentWildMonTable();
    void updateDiveMap(QString mapName);
    void updateEmergeMap(QString mapName);
    void setSelectedConnection(MapConnection *connection);

    void updatePrimaryTileset(QString tilesetLabel, bool forceLoad = false);
    void updateSecondaryTileset(QString tilesetLabel, bool forceLoad = false);
    void toggleBorderVisibility(bool visible, bool enableScriptCallback = true);
    void updateCustomMapAttributes();

    DraggablePixmapItem *addEventPixmapItem(Event *event);
    void removeEventPixmapItem(Event *event);
    bool canAddEvents(const QList<Event*> &events);
    void selectMapEvent(Event *event, bool toggle = false);
    Event *addNewEvent(Event::Type type);
    void updateEvents();
    void duplicateSelectedEvents();
    void redrawAllEvents();
    void redrawEvents(const QList<Event*> &events);
    void redrawEventPixmapItem(DraggablePixmapItem *item);
    qreal getEventOpacity(const Event *event) const;

    void updateCursorRectPos(int x, int y);
    void setCursorRectVisible(bool visible);

    void updateWarpEventWarning(Event *event);
    void updateWarpEventWarnings();

    QPointer<QGraphicsScene> scene = nullptr;
    QGraphicsPixmapItem *current_view = nullptr;
    QPointer<LayoutPixmapItem> map_item = nullptr;
    QList<QPointer<ConnectionPixmapItem>> connection_items;
    QMap<QString, QPointer<DivingMapPixmapItem>> diving_map_items;
    QGraphicsPathItem *connection_mask = nullptr;
    QPointer<CollisionPixmapItem> collision_item = nullptr;
    QGraphicsItemGroup *events_group = nullptr;

    QList<QGraphicsPixmapItem*> borderItems;
    QGraphicsItemGroup *mapGrid = nullptr;
    QPointer<MapRuler> map_ruler = nullptr;

    MovableRect *playerViewRect = nullptr;
    CursorTileRect *cursorMapTileRect = nullptr;

    QPointer<QGraphicsScene> scene_metatiles = nullptr;
    QPointer<QGraphicsScene> scene_current_metatile_selection = nullptr;
    QPointer<QGraphicsScene> scene_selected_border_metatiles = nullptr;
    QPointer<QGraphicsScene> scene_collision_metatiles = nullptr;
    QPointer<MetatileSelector> metatile_selector_item = nullptr;

    QPointer<BorderMetatilesPixmapItem> selected_border_metatiles_item = nullptr;
    CurrentSelectedMetatilesPixmapItem *current_metatile_selection_item = nullptr;
    QPointer<MovementPermissionsSelector> movement_permissions_selector_item = nullptr;

    QList<Event*> selectedEvents;
    QPointer<ConnectionPixmapItem> selected_connection_item = nullptr;
    QPointer<MapConnection> connection_to_select = nullptr;

    enum class EditAction { None, Paint, Select, Fill, Shift, Pick, Move };
    EditAction mapEditAction = EditAction::Paint;
    EditAction eventEditAction = EditAction::Select;

    enum class EditMode { None, Disabled, Metatiles, Collision, Header, Events, Connections, Encounters };
    void setEditMode(EditMode editMode);
    EditMode getEditMode() const { return this->editMode; }

    bool getEditingLayout();

    void setMapEditingButtonsEnabled(bool enabled);

    int scaleIndex = 2;
    qreal collisionOpacity = 0.5;
    static QList<QList<const QImage*>> collisionIcons;

    int eventShiftActionId = 0;

    void eventsView_onMousePress(QMouseEvent *event);

    int getBorderDrawDistance(int dimension);

    bool selectingEvent = false;

    void deleteSelectedEvents();
    void shouldReselectEvents();
    void scaleMapView(int);
    static void openInTextEditor(const QString &path, int lineNum = 0);
    void setCollisionGraphics();

public slots:
    void openMapScripts() const;
    void openScript(const QString &scriptLabel) const;
    void openProjectInTextEditor() const;
    void maskNonVisibleConnectionTiles();
    void onBorderMetatilesChanged();
    void selectedEventIndexChanged(int index, Event::Group eventGroup);
    void toggleGrid(bool);

private:
    const QImage defaultCollisionImgSheet = QImage(":/images/collisions.png");
    const QImage collisionPlaceholder = QImage(":/images/collisions_unknown.png");
    QPixmap collisionSheetPixmap;

    EditMode editMode = EditMode::None;

    void save(bool currentOnly);
    void clearMap();
    void clearMetatileSelector();
    void clearMovementPermissionSelector();
    void clearMapMetatiles();
    void clearMapMovementPermissions();
    void clearBorderMetatiles();
    void clearCurrentMetatilesSelection();
    void clearMapEvents();
    void clearMapConnections();
    void clearConnectionMask();
    void clearMapBorder();
    void clearMapGrid();
    void clearWildMonTables();
    int getSortedItemIndex(QComboBox *combo, QString item);
    void updateBorderVisibility();
    void removeConnectionPixmap(MapConnection *connection);
    void displayConnection(MapConnection *connection);
    void displayDivingConnection(MapConnection *connection);
    void setDivingMapName(QString mapName, QString direction);
    void removeDivingMapPixmap(MapConnection *connection);
    void updateEncounterFields(EncounterFields newFields);
    QString getMovementPermissionText(uint16_t collision, uint16_t elevation);
    QString getMetatileDisplayMessage(uint16_t metatileId);
    void setCollisionTabSpinBoxes(uint16_t collision, uint16_t elevation);
    static bool startDetachedProcess(const QString &command,
                                    const QString &workingDirectory = QString(),
                                    qint64 *pid = nullptr);

private slots:
    void onMapStartPaint(QGraphicsSceneMouseEvent *event, LayoutPixmapItem *item);
    void onMapEndPaint(QGraphicsSceneMouseEvent *event, LayoutPixmapItem *item);
    void setSmartPathCursorMode(QGraphicsSceneMouseEvent *event);
    void setStraightPathCursorMode(QGraphicsSceneMouseEvent *event);
    void mouseEvent_map(QGraphicsSceneMouseEvent *event, LayoutPixmapItem *item);
    void mouseEvent_collision(QGraphicsSceneMouseEvent *event, CollisionPixmapItem *item);
    void setSelectedConnectionItem(ConnectionPixmapItem *connectionItem);
    void onHoveredMovementPermissionChanged(uint16_t, uint16_t);
    void onHoveredMovementPermissionCleared();
    void onHoveredMetatileSelectionChanged(uint16_t);
    void onHoveredMetatileSelectionCleared();
    void onHoveredMapMetatileChanged(const QPoint &pos);
    void onHoveredMapMetatileCleared();
    void onHoveredMapMovementPermissionChanged(int, int);
    void onHoveredMapMovementPermissionCleared();
    void onSelectedMetatilesChanged();
    void onWheelZoom(int);

signals:
    void eventsChanged();
    void openConnectedMap(MapConnection*);
    void wildMonTableOpened(EncounterTableModel*);
    void wildMonTableClosed();
    void wildMonTableEdited();
    void warpEventDoubleClicked(QString, int, Event::Group);
    void currentMetatilesSelectionChanged();
    void mapRulerStatusChanged(const QString &);
    void tilesetUpdated(QString);
    void gridToggled(bool);
};

#endif // EDITOR_H
