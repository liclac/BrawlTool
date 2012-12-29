#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>

#define AssignPlayerControl(__array__, __suffix__, __player__) \
	__array__[(__player__ - 1)] = ui->p##__player__##__suffix__
#define AssignMapPlayerControl(__array__, __suffix__, __mapper__, __signal__, __player__) \
	AssignPlayerControl(__array__, __suffix__, __player__); \
	__mapper__.setMapping(ui->p##__player__##__suffix__, (__player__ - 1)); \
	connect(ui->p##__player__##__suffix__, SIGNAL(__signal__), &__mapper__, SLOT(map()))
#define AssignHelpTexts(__widget__) \
	__widget__->setStatusTip(__widget__->whatsThis()); \
	__widget__->setToolTip(__widget__->whatsThis())

#define BatchAssignPlayerControls(__array__, __suffix__) \
	AssignPlayerControl(__array__, __suffix__, 1); \
	AssignPlayerControl(__array__, __suffix__, 2); \
	AssignPlayerControl(__array__, __suffix__, 3); \
	AssignPlayerControl(__array__, __suffix__, 4)
#define BatchAssignMapPlayerControls(__array__, __suffix__, __mapper__, __signal__) \
	AssignMapPlayerControl(__array__, __suffix__, __mapper__, __signal__, 1); \
	AssignMapPlayerControl(__array__, __suffix__, __mapper__, __signal__, 2); \
	AssignMapPlayerControl(__array__, __suffix__, __mapper__, __signal__, 3); \
	AssignMapPlayerControl(__array__, __suffix__, __mapper__, __signal__, 4)

#define EnablePlayerControls(__array__, __condition__) \
	for(int __i__ = 0; __i__ < 4; __i__++) \
	__array__[__i__]->setEnabled((__condition__))
#define ExclusiveEnablePlayerControls(__array__, __currentPlayerIndex__) \
	EnablePlayerControls(__array__, __i__ == __currentPlayerIndex__)

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	currentlyPickingPlayer(-1)
{
	ui->setupUi(this);
	this->setWindowTitle(this->windowTitle() + " v" + qApp->applicationVersion());
	
	this->loadGrid(":/grids/Grid_Brawl.xml");
	this->resetGridTiles();
	
	BatchAssignPlayerControls(boxes, Box);
	BatchAssignPlayerControls(portraits, Portrait);
	BatchAssignPlayerControls(charNameLabels, CharName);
	//BatchAssignMapPlayerControls(changeButtons, ChangeButton, changeMapper, clicked());
	BatchAssignMapPlayerControls(selectButtons, SelectButton, selectMapper, clicked());
	BatchAssignMapPlayerControls(checkboxes, Check, checkMapper, stateChanged(int));
	BatchAssignPlayerControls(statsBoxes, StatsBox);
	BatchAssignPlayerControls(resetCountdownLabels, ResetCountdown);
	
	//connect(&changeMapper, SIGNAL(mapped(int)), this, SLOT(onChangePlayer(int)));
	connect(&selectMapper, SIGNAL(mapped(int)), this, SLOT(onSelectCharacters(int)));
	connect(&checkMapper, SIGNAL(mapped(int)), this, SLOT(onChangeParticipaction(int)));
	connect(&gridMapper, SIGNAL(mapped(int)), this, SLOT(onCharacterClicked(int)));
	connect(ui->reuseRuleResetOne, SIGNAL(toggled(bool)), this, SLOT(onReuseRuleChanged()));
	connect(ui->reuseRuleResetAll, SIGNAL(toggled(bool)), this, SLOT(onReuseRuleChanged()));
	connect(ui->reuseRuleAllow, SIGNAL(toggled(bool)), this, SLOT(onReuseRuleChanged()));
	
	onChangeParticipaction(0);
	onChangeParticipaction(1);
	onChangeParticipaction(2);
	onChangeParticipaction(3);
}

MainWindow::~MainWindow()
{
	//QSettings settings;
	//settings.beginWriteArray("players", 4);
	
	delete ui;
}

void MainWindow::loadGrid(QString filename)
{
	grid.portraitPrefix = "";
	grid.rows.clear();
	qDeleteAll(ui->gridContainer->findChildren<QWidget *>());
	
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this,
							 "Couldn't load Grid",
							 "The grid file could not be loaded. This is probably a bad thing.");
		return;
	}
	
	QXmlStreamReader reader(&file);
	int idCounter = 0;
	while(!reader.atEnd())
	{
		QXmlStreamReader::TokenType type = reader.readNext();
		if(type == QXmlStreamReader::StartElement)
		{
			if(reader.name() == "grid")
			{
				grid.portraitPrefix = reader.attributes().value("portraitPrefix").toString();
				grid.portraitSuffix = reader.attributes().value("portraitSuffix").toString();
				grid.spacerPortrait = reader.attributes().value("spacerPortrait").toString();
			}
			else if(reader.name() == "row")
				grid.rows.append(QList<int>());
			else if(reader.name() == "char")
			{
				Character currentChar;
				currentChar.isSpacer = false;
				currentChar.id = idCounter;
				currentChar.name = reader.attributes().value("name").toString();
				currentChar.portrait = reader.attributes().value("portrait").toString();
				grid.chars.insert(currentChar.id, currentChar);
				grid.rows.last().append(currentChar.id);
				
				idCounter++;
			}
			else if(reader.name() == "spacer")
			{
				Character spacerChar;
				spacerChar.isSpacer = true;
				spacerChar.id = idCounter;
				spacerChar.name = "";
				spacerChar.portrait = grid.spacerPortrait;
				grid.chars.insert(spacerChar.id, spacerChar);
				grid.rows.last().append(spacerChar.id);
				
				idCounter++;
			}
		}
	}
	
	foreach(QList<int> row, grid.rows)
	{
		QWidget *rowWidget = new QWidget(ui->gridContainer);
		QHBoxLayout *rowLayout = new QHBoxLayout;
		
		foreach(int id, row)
		{
			Character chr = grid.chars.value(id);
			QString portraitPath = ":/portraits/" + grid.portraitPrefix + chr.portrait + grid.portraitSuffix;
			//qDebug() << portraitPath;
			
			ClickableLabel *label = new ClickableLabel();
			if(!chr.isSpacer) label->setPixmap(QPixmap(portraitPath));
			label->setStatusTip(chr.name);
			label->setMinimumSize(50, 50);
			rowLayout->addWidget(label);
			grid.tiles.insert(chr.id, label);
			
			if(!chr.isSpacer)
			{
				gridMapper.setMapping(label, chr.id);
				connect(label, SIGNAL(clicked()), &gridMapper, SLOT(map()));
			}
		}
		
		rowWidget->setLayout(rowLayout);
		ui->gridContainer->layout()->addWidget(rowWidget);
	}
}

void MainWindow::resetGridTiles()
{
	foreach(int id, grid.chars.keys())
		grid.tiles.value(id)->setGray(false);
}

void MainWindow::updateGridTiles(QList<int> charsEnabled)
{
	foreach(int id, grid.chars.keys())
		grid.tiles.value(id)->setGray(true);
	foreach(int id, charsEnabled)
		grid.tiles.value(id)->setGray(false);
}

void MainWindow::onCharacterClicked(int index)
{
	if(currentlyPickingPlayer == -1) return;
	
	QList<int> *chars = &(players[currentlyPickingPlayer].charsEnabled);
	ClickableLabel *tile = grid.tiles.value(index);
	if(!chars->contains(index))
	{
		tile->setGray(false);
		chars->append(index);
	}
	else
	{
		tile->setGray(true);
		chars->removeAll(index);
	}
}

void MainWindow::onChangePlayer(int index)
{
	Q_UNUSED(index)
}

void MainWindow::onSelectCharacters(int index)
{
	if(currentlyPickingPlayer == -1)
	{
		currentlyPickingPlayer = index;
		ExclusiveEnablePlayerControls(boxes, index);
		selectButtons[index]->setDefault(true);
		//changeButtons[index]->setEnabled(false);
		checkboxes[index]->setEnabled(false);
		ui->rollButton->setEnabled(false);
		this->updateGridTiles(players[index].charsEnabled);
	}
	else
	{
		players[currentlyPickingPlayer].charsPooled.clear();
		players[currentlyPickingPlayer].charsPooled.append(players[currentlyPickingPlayer].charsEnabled);
		resetCountdownLabels[currentlyPickingPlayer]->
				setText(QString::number(players[currentlyPickingPlayer].charsPooled.length()));
		
		currentlyPickingPlayer = -1;
		EnablePlayerControls(boxes, true);
		selectButtons[index]->setDefault(false);
		//changeButtons[index]->setEnabled(true);
		checkboxes[index]->setEnabled(true);
		ui->rollButton->setEnabled(true);
		this->resetGridTiles();
	}
}

void MainWindow::onChangeParticipaction(int index)
{
	bool state = checkboxes[index]->isChecked();
	portraits[index]->setEnabled(state);
	//changeButtons[index]->setEnabled(state);
	selectButtons[index]->setEnabled(state);
}

void MainWindow::on_rollButton_clicked()
{
	for(int i = 0; i < 4; i++)
	{
		if(checkboxes[i]->isChecked() && players[i].charsEnabled.length() == 0)
		{
			QMessageBox::warning(this,
								 "Player " + QString::number(i+1) + " has not yet picked characters!",
								 "You can only roll once all participating players have picked "
								 "at least one character to use.\n\n"
								 "Player " + QString::number(i+1) + " has not yet picked any.");
			return;
		}
	}
	
	for(int i = 0; i < 4; i++)
	{
		if(!checkboxes[i]->isChecked()) continue;	//Skip non-participating players
		
		if(players[i].charsPooled.length() == 0)
		{
			if(ui->reuseRuleResetOne->isChecked()) players[i].charsPooled.append(players[i].charsEnabled);
			else if(ui->reuseRuleResetAll->isChecked())
			{
				for(int j = 0; j < 4; j++)
				{
					players[j].charsPooled.clear();
					players[j].charsPooled.append(players[j].charsEnabled);
				}
			}
		}
		
		int index = qrand() % (players[i].charsPooled.length());
		Character chr = grid.chars.value(players[i].charsPooled.at(index));
		QString portraitPath = ":/portraits/" + grid.portraitPrefix + chr.portrait + grid.portraitSuffix;	
		portraits[i]->setPixmap(QPixmap(portraitPath));
		portraits[i]->setStatusTip(chr.name);
		charNameLabels[i]->setText(chr.name);
		
		if(!ui->reuseRuleAllow->isChecked())
		{
			players[i].charsPooled.removeAt(index);
			resetCountdownLabels[i]->setText(QString::number(players[i].charsPooled.length()));
		}
	}
}

void MainWindow::onReuseRuleChanged()
{
	for(int j = 0; j < 4; j++)
	{
		players[j].charsPooled.clear();
		players[j].charsPooled.append(players[j].charsEnabled);
		statsBoxes[j]->setVisible(!ui->reuseRuleAllow->isChecked());
		boxes[j]->updateGeometry();
	}
}
