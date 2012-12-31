#ifndef MACROS_H
#define MACROS_H

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

#endif // MACROS_H
