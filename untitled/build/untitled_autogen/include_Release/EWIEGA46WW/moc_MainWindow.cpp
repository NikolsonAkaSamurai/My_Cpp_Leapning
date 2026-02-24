/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../MainWindow.h"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "onNewProfile",
    "",
    "onDeleteProfile",
    "onRenameProfile",
    "onDuplicateProfile",
    "onProfileSelected",
    "QListWidgetItem*",
    "item",
    "onStartSearch",
    "onSearchStarted",
    "query",
    "total",
    "onPageParsed",
    "page",
    "count",
    "onVacancyParsed",
    "Vacancy",
    "v",
    "onSearchFinished",
    "QList<Vacancy>",
    "vacancies",
    "onVacancySelected",
    "onFullVacancyFetched",
    "showVacancyDetails",
    "onShowChart",
    "onShowCryptoChart",
    "onBrowseVacancies",
    "onExportJson",
    "onExportCsv",
    "onShowStats",
    "onProfileLoaded",
    "name",
    "onProfileCreated",
    "onProfileDeleted",
    "onProfileRenamed",
    "oldName",
    "newName",
    "onParserError",
    "error"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {
    uint offsetsAndSizes[80];
    char stringdata0[11];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[16];
    char stringdata5[19];
    char stringdata6[18];
    char stringdata7[17];
    char stringdata8[5];
    char stringdata9[14];
    char stringdata10[16];
    char stringdata11[6];
    char stringdata12[6];
    char stringdata13[13];
    char stringdata14[5];
    char stringdata15[6];
    char stringdata16[16];
    char stringdata17[8];
    char stringdata18[2];
    char stringdata19[17];
    char stringdata20[15];
    char stringdata21[10];
    char stringdata22[18];
    char stringdata23[21];
    char stringdata24[19];
    char stringdata25[12];
    char stringdata26[18];
    char stringdata27[18];
    char stringdata28[13];
    char stringdata29[12];
    char stringdata30[12];
    char stringdata31[16];
    char stringdata32[5];
    char stringdata33[17];
    char stringdata34[17];
    char stringdata35[17];
    char stringdata36[8];
    char stringdata37[8];
    char stringdata38[14];
    char stringdata39[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMainWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMainWindowENDCLASS_t qt_meta_stringdata_CLASSMainWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 12),  // "onNewProfile"
        QT_MOC_LITERAL(24, 0),  // ""
        QT_MOC_LITERAL(25, 15),  // "onDeleteProfile"
        QT_MOC_LITERAL(41, 15),  // "onRenameProfile"
        QT_MOC_LITERAL(57, 18),  // "onDuplicateProfile"
        QT_MOC_LITERAL(76, 17),  // "onProfileSelected"
        QT_MOC_LITERAL(94, 16),  // "QListWidgetItem*"
        QT_MOC_LITERAL(111, 4),  // "item"
        QT_MOC_LITERAL(116, 13),  // "onStartSearch"
        QT_MOC_LITERAL(130, 15),  // "onSearchStarted"
        QT_MOC_LITERAL(146, 5),  // "query"
        QT_MOC_LITERAL(152, 5),  // "total"
        QT_MOC_LITERAL(158, 12),  // "onPageParsed"
        QT_MOC_LITERAL(171, 4),  // "page"
        QT_MOC_LITERAL(176, 5),  // "count"
        QT_MOC_LITERAL(182, 15),  // "onVacancyParsed"
        QT_MOC_LITERAL(198, 7),  // "Vacancy"
        QT_MOC_LITERAL(206, 1),  // "v"
        QT_MOC_LITERAL(208, 16),  // "onSearchFinished"
        QT_MOC_LITERAL(225, 14),  // "QList<Vacancy>"
        QT_MOC_LITERAL(240, 9),  // "vacancies"
        QT_MOC_LITERAL(250, 17),  // "onVacancySelected"
        QT_MOC_LITERAL(268, 20),  // "onFullVacancyFetched"
        QT_MOC_LITERAL(289, 18),  // "showVacancyDetails"
        QT_MOC_LITERAL(308, 11),  // "onShowChart"
        QT_MOC_LITERAL(320, 17),  // "onShowCryptoChart"
        QT_MOC_LITERAL(338, 17),  // "onBrowseVacancies"
        QT_MOC_LITERAL(356, 12),  // "onExportJson"
        QT_MOC_LITERAL(369, 11),  // "onExportCsv"
        QT_MOC_LITERAL(381, 11),  // "onShowStats"
        QT_MOC_LITERAL(393, 15),  // "onProfileLoaded"
        QT_MOC_LITERAL(409, 4),  // "name"
        QT_MOC_LITERAL(414, 16),  // "onProfileCreated"
        QT_MOC_LITERAL(431, 16),  // "onProfileDeleted"
        QT_MOC_LITERAL(448, 16),  // "onProfileRenamed"
        QT_MOC_LITERAL(465, 7),  // "oldName"
        QT_MOC_LITERAL(473, 7),  // "newName"
        QT_MOC_LITERAL(481, 13),  // "onParserError"
        QT_MOC_LITERAL(495, 5)   // "error"
    },
    "MainWindow",
    "onNewProfile",
    "",
    "onDeleteProfile",
    "onRenameProfile",
    "onDuplicateProfile",
    "onProfileSelected",
    "QListWidgetItem*",
    "item",
    "onStartSearch",
    "onSearchStarted",
    "query",
    "total",
    "onPageParsed",
    "page",
    "count",
    "onVacancyParsed",
    "Vacancy",
    "v",
    "onSearchFinished",
    "QList<Vacancy>",
    "vacancies",
    "onVacancySelected",
    "onFullVacancyFetched",
    "showVacancyDetails",
    "onShowChart",
    "onShowCryptoChart",
    "onBrowseVacancies",
    "onExportJson",
    "onExportCsv",
    "onShowStats",
    "onProfileLoaded",
    "name",
    "onProfileCreated",
    "onProfileDeleted",
    "onProfileRenamed",
    "oldName",
    "newName",
    "onParserError",
    "error"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  158,    2, 0x08,    1 /* Private */,
       3,    0,  159,    2, 0x08,    2 /* Private */,
       4,    0,  160,    2, 0x08,    3 /* Private */,
       5,    0,  161,    2, 0x08,    4 /* Private */,
       6,    1,  162,    2, 0x08,    5 /* Private */,
       9,    0,  165,    2, 0x08,    7 /* Private */,
      10,    2,  166,    2, 0x08,    8 /* Private */,
      13,    3,  171,    2, 0x08,   11 /* Private */,
      16,    1,  178,    2, 0x08,   15 /* Private */,
      19,    1,  181,    2, 0x08,   17 /* Private */,
      22,    1,  184,    2, 0x08,   19 /* Private */,
      23,    1,  187,    2, 0x08,   21 /* Private */,
      24,    1,  190,    2, 0x08,   23 /* Private */,
      25,    0,  193,    2, 0x08,   25 /* Private */,
      26,    0,  194,    2, 0x08,   26 /* Private */,
      27,    0,  195,    2, 0x08,   27 /* Private */,
      28,    0,  196,    2, 0x08,   28 /* Private */,
      29,    0,  197,    2, 0x08,   29 /* Private */,
      30,    0,  198,    2, 0x08,   30 /* Private */,
      31,    2,  199,    2, 0x08,   31 /* Private */,
      33,    1,  204,    2, 0x08,   34 /* Private */,
      34,    1,  207,    2, 0x08,   36 /* Private */,
      35,    2,  210,    2, 0x08,   38 /* Private */,
      38,    1,  215,    2, 0x08,   41 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   11,   12,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   14,   12,   15,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   32,   15,
    QMetaType::Void, QMetaType::QString,   32,
    QMetaType::Void, QMetaType::QString,   32,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   36,   37,
    QMetaType::Void, QMetaType::QString,   39,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'onNewProfile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeleteProfile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRenameProfile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDuplicateProfile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onProfileSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'onStartSearch'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSearchStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onPageParsed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onVacancyParsed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Vacancy &, std::false_type>,
        // method 'onSearchFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QList<Vacancy> &, std::false_type>,
        // method 'onVacancySelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'onFullVacancyFetched'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Vacancy &, std::false_type>,
        // method 'showVacancyDetails'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Vacancy &, std::false_type>,
        // method 'onShowChart'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onShowCryptoChart'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onBrowseVacancies'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExportJson'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExportCsv'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onShowStats'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onProfileLoaded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onProfileCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onProfileDeleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onProfileRenamed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onParserError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onNewProfile(); break;
        case 1: _t->onDeleteProfile(); break;
        case 2: _t->onRenameProfile(); break;
        case 3: _t->onDuplicateProfile(); break;
        case 4: _t->onProfileSelected((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 5: _t->onStartSearch(); break;
        case 6: _t->onSearchStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 7: _t->onPageParsed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 8: _t->onVacancyParsed((*reinterpret_cast< std::add_pointer_t<Vacancy>>(_a[1]))); break;
        case 9: _t->onSearchFinished((*reinterpret_cast< std::add_pointer_t<QList<Vacancy>>>(_a[1]))); break;
        case 10: _t->onVacancySelected((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 11: _t->onFullVacancyFetched((*reinterpret_cast< std::add_pointer_t<Vacancy>>(_a[1]))); break;
        case 12: _t->showVacancyDetails((*reinterpret_cast< std::add_pointer_t<Vacancy>>(_a[1]))); break;
        case 13: _t->onShowChart(); break;
        case 14: _t->onShowCryptoChart(); break;
        case 15: _t->onBrowseVacancies(); break;
        case 16: _t->onExportJson(); break;
        case 17: _t->onExportCsv(); break;
        case 18: _t->onShowStats(); break;
        case 19: _t->onProfileLoaded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 20: _t->onProfileCreated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 21: _t->onProfileDeleted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->onProfileRenamed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 23: _t->onParserError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 24;
    }
    return _id;
}
QT_WARNING_POP
