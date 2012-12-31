#include "Tools.h"

QList<int> splitStringToInts(QString &str)
{
	if(str.length() == 0) return QList<int>();
	
	QList<int> ret;
	QStringList components = str.split('|');
	foreach(QString component, components)
		ret.append(component.toInt());
	
	return ret;
}

QString joinIntListToString(QList<int> &list)
{
	if(list.length() == 0) return QString();
	
	QString ret(QString::number(list.at(0)));
	for(int i = 1; i < list.length(); i++)
	{
		ret.append("|");
		ret.append(QString::number(list.at(i)));
	}
	
	return ret;
}
