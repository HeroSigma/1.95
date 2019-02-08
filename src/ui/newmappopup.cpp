#include "newmappopup.h"
#include "event.h"
#include "maplayout.h"
#include "mainwindow.h"
#include "ui_newmappopup.h"
#include "config.h"

#include <QMap>
#include <QSet>
#include <QPalette>
#include <QStringList>

NewMapPopup::NewMapPopup(QWidget *parent, Project *project) :
    QMainWindow(parent),
    ui(new Ui::NewMapPopup)
{
    ui->setupUi(this);
    this->project = project;
    this->existingLayout = false;
}

NewMapPopup::~NewMapPopup()
{
    delete ui;
}

void NewMapPopup::init(int type, int group, QString sec, QString layoutId) {
    switch (type)
    {
        case MapSortOrder::Group:
            setDefaultValues(group, QString());
            break;
        case MapSortOrder::Area:
            setDefaultValues(group, sec);
            break;
        case MapSortOrder::Layout:
            useLayout(layoutId);
            setDefaultValues(group, QString());
            break;
    }
}

void NewMapPopup::useLayout(QString layoutId) {
    this->existingLayout = true;
    this->layoutId = layoutId;
}

void NewMapPopup::setDefaultValues(int groupNum, QString mapSec) {
    ui->lineEdit_NewMap_Name->setText(project->getNewMapName());

    QMap<QString, QStringList> tilesets = project->getTilesets();
    ui->comboBox_NewMap_Primary_Tileset->addItems(tilesets.value("primary"));
    ui->comboBox_NewMap_Secondary_Tileset->addItems(tilesets.value("secondary"));

    ui->comboBox_NewMap_Group->addItems(*project->groupNames);
    ui->comboBox_NewMap_Group->setCurrentText("gMapGroup" + QString::number(groupNum));

    if (existingLayout) {
        ui->spinBox_NewMap_Width->setValue(project->mapLayouts.value(layoutId)->width.toInt(nullptr, 0));
        ui->spinBox_NewMap_Height->setValue(project->mapLayouts.value(layoutId)->height.toInt(nullptr, 0));
        ui->comboBox_NewMap_Primary_Tileset->setCurrentText(project->mapLayouts.value(layoutId)->tileset_primary_label);
        ui->comboBox_NewMap_Secondary_Tileset->setCurrentText(project->mapLayouts.value(layoutId)->tileset_secondary_label);
        ui->spinBox_NewMap_Width->setDisabled(true);
        ui->spinBox_NewMap_Height->setDisabled(true);
        ui->comboBox_NewMap_Primary_Tileset->setDisabled(true);
        ui->comboBox_NewMap_Secondary_Tileset->setDisabled(true);
    } else {
        ui->spinBox_NewMap_Width->setValue(20);
        ui->spinBox_NewMap_Height->setValue(20);
    }

    ui->comboBox_NewMap_Type->addItems(*project->mapTypes);
    ui->comboBox_NewMap_Location->addItems(*project->regionMapSections);
    if (!mapSec.isEmpty()) ui->comboBox_NewMap_Location->setCurrentText(mapSec);

    ui->frame_NewMap_Options->setEnabled(true);

    switch (projectConfig.getBaseGameVersion())
    {
    case BaseGameVersion::pokeruby:
        ui->checkBox_NewMap_Allow_Running->setVisible(false);
        ui->checkBox_NewMap_Allow_Biking->setVisible(false);
        ui->checkBox_NewMap_Allow_Escape_Rope->setVisible(false);
        ui->label_NewMap_Allow_Running->setVisible(false);
        ui->label_NewMap_Allow_Biking->setVisible(false);
        ui->label_NewMap_Allow_Escape_Rope->setVisible(false);
        break;
    case BaseGameVersion::pokeemerald:
        ui->checkBox_NewMap_Allow_Running->setVisible(true);
        ui->checkBox_NewMap_Allow_Biking->setVisible(true);
        ui->checkBox_NewMap_Allow_Escape_Rope->setVisible(true);
        ui->label_NewMap_Allow_Running->setVisible(true);
        ui->label_NewMap_Allow_Biking->setVisible(true);
        ui->label_NewMap_Allow_Escape_Rope->setVisible(true);
        break;
    }
}

void NewMapPopup::on_lineEdit_NewMap_Name_textChanged(const QString &text) {
    if (project->mapNames->contains(text)) {
        QPalette palette = this->ui->lineEdit_NewMap_Name->palette();
        QColor color = Qt::red;
        color.setAlpha(25);
        palette.setColor(QPalette::Base, color);
        this->ui->lineEdit_NewMap_Name->setPalette(palette);
    } else {
        this->ui->lineEdit_NewMap_Name->setPalette(QPalette());
    }
}

void NewMapPopup::on_pushButton_NewMap_Accept_clicked() {
    Map *newMap = new Map;
    MapLayout *layout;

    // If map name is not unique, use default value. Also use only valid characters.
    QString newMapName = this->ui->lineEdit_NewMap_Name->text().remove(QRegularExpression("[^a-zA-Z0-9_]+"));
    if (project->mapNames->contains(newMapName) || newMapName.isEmpty()) {
        newMapName = project->getNewMapName();
    }

    newMap->name = newMapName;
    newMap->type = this->ui->comboBox_NewMap_Type->currentText();
    newMap->location = this->ui->comboBox_NewMap_Location->currentText();
    newMap->song = "MUS_DAN02";
    newMap->requiresFlash = "0";
    newMap->weather = "WEATHER_SUNNY";
    newMap->show_location = "1";
    newMap->battle_scene = "MAP_BATTLE_SCENE_NORMAL";

    if (this->existingLayout) {
        layout = this->project->mapLayouts.value(this->layoutId);
    } else {
        layout = new MapLayout;
        layout->id = MapLayout::layoutConstantFromName(newMapName);
        layout->name = QString("%1_Layout").arg(newMap->name);
        layout->width = QString::number(this->ui->spinBox_NewMap_Width->value());
        layout->height = QString::number(this->ui->spinBox_NewMap_Height->value());
        layout->tileset_primary_label = this->ui->comboBox_NewMap_Primary_Tileset->currentText();
        layout->tileset_secondary_label = this->ui->comboBox_NewMap_Secondary_Tileset->currentText();
        layout->border_path = QString("data/layouts/%1/border.bin").arg(newMapName);
        layout->blockdata_path = QString("data/layouts/%1/map.bin").arg(newMapName);
    }

    if (this->ui->checkBox_NewMap_Flyable->isChecked()) {
        newMap->isFlyable = "TRUE";
    }

    if (projectConfig.getBaseGameVersion() == BaseGameVersion::pokeemerald) {
        newMap->allowRunning = this->ui->checkBox_NewMap_Allow_Running->isChecked() ? "1" : "0";
        newMap->allowBiking = this->ui->checkBox_NewMap_Allow_Biking->isChecked() ? "1" : "0";
        newMap->allowEscapeRope = this->ui->checkBox_NewMap_Allow_Escape_Rope->isChecked() ? "1" : "0";
    }

    group = this->ui->comboBox_NewMap_Group->currentText().remove("gMapGroup").toInt();
    newMap->layout = layout;
    newMap->layoutId = layout->id;
    project->loadMapLayout(newMap);
    newMap->group_num = QString::number(group);
    map = newMap;
    emit applied();
    this->close();
}
