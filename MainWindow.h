#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtXml>

#include "ClickableLabel.h"
#include "Tools.h"
#include "InvisibilityFilter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
	struct Character {
		bool isSpacer;
		int id;				//Yes, Spacers get IDs. Deal with it. As in, don't move them around too much.
		QString name;
		QString portrait;
	};
	
	struct Grid {
		QString path;
		QString portraitPrefix, portraitSuffix, spacerPortrait;
		QMap<int, Character> chars;
		QList<QList<int> > rows;					//Values are Character.id values
		QMap<int, ClickableLabel*> tiles;			//Keys are Character.id values
	};
	
	struct Player {
		QList<int> charsEnabled, charsPooled;		//Keys are Character.id values
		int currentChar, _oldChar;
		bool participating;
		
		//gah, I just want to memset this!
		Player() :
			currentChar(0),
			_oldChar(-1),
			participating(false)
		{}
	};
	
	enum ReuseRule {
		ResetOne,
		ResetAll,
		AllowReuse
	};
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	
	void loadGrid(QString filename);
	
private:
	void initUI();
	void updateGrid();
	void updatePlayers();
	void updatePlayer(int index);
	void refillPools();
	void refillPool(int index);
	
protected:
	//-- UI
	QGroupBox *boxes[4];
	QLabel *portraits[4];
	QLabel *charNameLabels[4];
	//QPushButton *changeButtons[4];
	QPushButton *selectButtons[4];
	QCheckBox *checkboxes[4];
	
	QGroupBox *statsBoxes[4];
	QLabel *resetCountdownLabels[4];
	
	QSignalMapper changeMapper, selectMapper, checkMapper;
	QSignalMapper gridMapper;
	
	InvisibilityFilter invisibilityFilter;
	
	//-- Data
	Grid grid;
	Player players[4];
	int currentlyPickingPlayer;		//-1 = no one is picking
	ReuseRule reuseRule;
	
private slots:
	void onCharacterClicked(int index);
	void onChangePlayer(int index);
	void onSelectCharacters(int index);
	void onChangeParticipaction(int index);
	void onReuseRuleChanged();
	
	void on_rollButton_clicked();
	void on_clearButton_clicked();
	void on_resetNowButton_clicked();
	
private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
