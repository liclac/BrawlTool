#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtXml>

#include "ClickableLabel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
	struct Character {
		bool isSpacer;
		int id;
		QString name;
		QString portrait;
	};
	
	struct Grid {
		QString portraitPrefix, portraitSuffix, spacerPortrait;
		QMap<int, Character> chars;
		QList<QList<int> > rows;					//Values are Character.id values
		QMap<int, ClickableLabel*> tiles;			//Keys are Character.id values
	};
	
	struct Player {
		QList<int> charsEnabled, charsPooled;		//Keys are Character.id values
	};
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	
	void loadGrid(QString filename);
	void resetGridTiles();									//Un-grays everything
	void updateGridTiles(QList<int> charsEnabled);
	
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
	
	//-- Data
	Grid grid;
	Player players[4];
	int currentlyPickingPlayer;		//-1 = no one is picking
	
private slots:
	void onCharacterClicked(int index);
	
	//NOTE: These will be called with array indices, not player numbers
	//so, Player 1 is 0, not 1
	void onChangePlayer(int index);
	void onSelectCharacters(int index);
	void onChangeParticipaction(int index);
	
	void on_rollButton_clicked();
	
	void onReuseRuleChanged();
	
private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
