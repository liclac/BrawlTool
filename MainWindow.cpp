#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
#include "Macros.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	currentlyPickingPlayer(-1)
{
	ui->setupUi(this);
	this->setWindowTitle(this->windowTitle() + " v" + qApp->applicationVersion());
	
	players[0].participating = true;
	players[1].participating = true;
	
	QSettings settings;
	settings.beginGroup("state");
	{
		this->loadGrid(settings.value("gridPath", ":/grids/Grid_Brawl.xml").toString());
		
		reuseRule = (ReuseRule)settings.value("reuseRule", ResetOne).toInt();
		ui->reuseRuleResetOne->setChecked(reuseRule == ResetOne);
		ui->reuseRuleResetAll->setChecked(reuseRule == ResetAll);
		ui->reuseRuleAllow->setChecked(reuseRule == AllowReuse);
		ui->resetNowButton->setVisible(reuseRule != AllowReuse);
		
		settings.beginReadArray("players");
		{
			for(int i = 0; i < 4; i++)
			{
				qDebug() << "Reading Player" << i;
				qDebug() << "- Enabled:" << settings.value("charsEnabled").toString();
				qDebug() << "- Pooled:" << settings.value("charsPooled").toString();
				
				Player &player = players[i];
				settings.setArrayIndex(i);
				player.charsEnabled = splitStringToInts(settings.value("charsEnabled").toString());
				player.charsPooled = splitStringToInts(settings.value("charsPooled").toString());
				player.currentChar = settings.value("currentChar", player.currentChar).toInt();
				player._oldChar = -1; //Force UI Refresh
				player.participating = settings.value("participating", player.participating).toBool();
			}
		}
		settings.endArray();
	}
	settings.endGroup();
	
	this->initUI();
	this->updateGrid();
	this->updatePlayers();
	ui->rollButton->setFocus();
}

MainWindow::~MainWindow()
{
	QSettings settings;
	
	settings.beginGroup("state");
	{
		settings.setValue("gridPath", grid.path);
		settings.setValue("reuseRule", reuseRule);
		
		settings.beginWriteArray("players");
		{
			for(int i = 0; i < 4; i++)
			{
				Player &player = players[i];
				qDebug() << "Writing Player" << i;
				qDebug() << "- Enabled:" << joinIntListToString(player.charsEnabled);
				qDebug() << "- Pooled:" << joinIntListToString(player.charsPooled);
				
				settings.setArrayIndex(i);
				settings.setValue("charsEnabled", joinIntListToString(player.charsEnabled));
				settings.setValue("charsPooled", joinIntListToString(player.charsPooled));
				settings.setValue("currentChar", player.currentChar);
				settings.setValue("participating", player.participating);
			}
		}
		settings.endArray();
	}
	settings.endGroup();
	
	delete ui;
}

void MainWindow::loadGrid(QString filename)
{
	grid.path = filename;
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
	int defaultChar = -1;	//-1 = no character has yet been found
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
				
				if(defaultChar == -1) defaultChar = currentChar.id;
				
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

void MainWindow::initUI()
{
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
}

void MainWindow::updateGrid()
{
	if(currentlyPickingPlayer == -1)
	{
		ui->gridContainer->setUpdatesEnabled(false);
		{
			foreach(int id, grid.chars.keys())
				grid.tiles.value(id)->setGray(false);
		}
		ui->gridContainer->setUpdatesEnabled(true);
	}
	else
	{
		ui->gridContainer->setUpdatesEnabled(false);
		{
			foreach(int id, grid.chars.keys())
				grid.tiles.value(id)->setGray(true);
			foreach(int id, players[currentlyPickingPlayer].charsEnabled)
				grid.tiles.value(id)->setGray(false);
		}
		ui->gridContainer->setUpdatesEnabled(true);
	}
}

void MainWindow::updatePlayers()
{
	for(int i = 0; i < 4; i++)	//Yes, this will be unrolled by the compiler. Hopefully.
		this->updatePlayer(i);
}

void MainWindow::updatePlayer(int index)
{
	qDebug() << "Updating Player" << index;
	Player &player = players[index];
	
	portraits[index]->setEnabled(player.participating);
	charNameLabels[index]->setEnabled(player.participating);
	selectButtons[index]->setEnabled(player.participating);
	statsBoxes[index]->setEnabled(player.participating);
	
	checkboxes[index]->setChecked(player.participating);	//Make sure this is correct at launch
	
	if(player._oldChar != player.currentChar)	//Don't update this unless it changed
	{
		Character chr = grid.chars.value(player.currentChar);
		qDebug() << "- " << chr.name;
		QString portraitPath = ":/portraits/" + grid.portraitPrefix + chr.portrait + grid.portraitSuffix;	
		portraits[index]->setPixmap(QPixmap(portraitPath));
		portraits[index]->setStatusTip(chr.name);
		charNameLabels[index]->setText(chr.name);
		player._oldChar = player.currentChar;
	}
	
	//statsBoxes[index]->setVisible(reuseRule != AllowReuse);
	invisibilityFilter.setWidgetVisible(statsBoxes[index], (reuseRule != AllowReuse));
	resetCountdownLabels[index]->setText(QString::number(players[index].charsPooled.length()));
}

void MainWindow::refillPools()
{
	for(int i = 0; i < 4; i++)
		this->refillPool(i);
}

void MainWindow::refillPool(int index)
{
	players[index].charsPooled.clear();
	players[index].charsPooled.append(players[index].charsEnabled);
	this->updatePlayer(index);
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
		ui->clearButton->setEnabled(false);
		ui->resetNowButton->setEnabled(false);
		
		this->updateGrid();
	}
	else
	{
		qSort(players[index].charsEnabled);	//Because why not
		
		currentlyPickingPlayer = -1;
		EnablePlayerControls(boxes, true);
		selectButtons[index]->setDefault(false);
		//changeButtons[index]->setEnabled(true);
		checkboxes[index]->setEnabled(true);
		ui->rollButton->setEnabled(true);
		ui->clearButton->setEnabled(true);
		ui->resetNowButton->setEnabled(true);
		
		this->updateGrid();
		this->refillPool(index);
		this->updatePlayer(index);
	}
}

void MainWindow::onChangeParticipaction(int index)
{
	players[index].participating = checkboxes[index]->isChecked();
	this->updatePlayer(index);
}

void MainWindow::onReuseRuleChanged()
{
	if(ui->reuseRuleResetOne->isChecked()) reuseRule = ResetOne;
	else if(ui->reuseRuleResetAll->isChecked()) reuseRule = ResetAll;
	else reuseRule = AllowReuse;
	ui->resetNowButton->setVisible(reuseRule != AllowReuse);
	
	this->refillPools();
}

void MainWindow::on_rollButton_clicked()
{
	for(int i = 0; i < 4; i++)
	{
		if(players[i].participating && players[i].charsEnabled.length() == 0)
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
		if(!players[i].participating) continue;	//Skip non-participating players
		
		if(players[i].charsPooled.length() == 0)
		{
			if(reuseRule == ResetOne) this->refillPool(i);
			else if(reuseRule == ResetAll) this->refillPools();
		}
		
		int index = qrand() % (players[i].charsPooled.length());
		players[i].currentChar = players[i].charsPooled.at(index);
		if(reuseRule != AllowReuse) players[i].charsPooled.removeAt(index);
		this->updatePlayer(i);	//This is faster than updating everything afterwards,
								//because I get one less loop
	}
}

void MainWindow::on_clearButton_clicked()
{
	for(int i = 0; i < 4; i++)
	{
		players[i] = Player();
		if(i < 2) players[i].participating = true;
		this->updatePlayers();
	}
}

void MainWindow::on_resetNowButton_clicked()
{
    this->refillPools();
}
