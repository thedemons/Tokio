/****************************************************************************
** Meta object code from reading C++ file 'TextView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../TextView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TextView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_nc__gui__TextView_t {
    QByteArrayData data[25];
    char stringdata0[284];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_nc__gui__TextView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_nc__gui__TextView_t qt_meta_stringdata_nc__gui__TextView = {
    {
QT_MOC_LITERAL(0, 0, 17), // "nc::gui::TextView"
QT_MOC_LITERAL(1, 18, 18), // "contextMenuCreated"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 6), // "QMenu*"
QT_MOC_LITERAL(4, 45, 4), // "menu"
QT_MOC_LITERAL(5, 50, 6), // "status"
QT_MOC_LITERAL(6, 57, 7), // "message"
QT_MOC_LITERAL(7, 65, 9), // "highlight"
QT_MOC_LITERAL(8, 75, 24), // "std::vector<Range<int> >"
QT_MOC_LITERAL(9, 100, 6), // "ranges"
QT_MOC_LITERAL(10, 107, 13), // "ensureVisible"
QT_MOC_LITERAL(11, 121, 10), // "moveCursor"
QT_MOC_LITERAL(12, 132, 8), // "position"
QT_MOC_LITERAL(13, 141, 6), // "saveAs"
QT_MOC_LITERAL(14, 148, 6), // "zoomIn"
QT_MOC_LITERAL(15, 155, 5), // "delta"
QT_MOC_LITERAL(16, 161, 7), // "zoomOut"
QT_MOC_LITERAL(17, 169, 15), // "setDocumentFont"
QT_MOC_LITERAL(18, 185, 4), // "font"
QT_MOC_LITERAL(19, 190, 10), // "selectFont"
QT_MOC_LITERAL(20, 201, 21), // "updateExtraSelections"
QT_MOC_LITERAL(21, 223, 20), // "updatePositionStatus"
QT_MOC_LITERAL(22, 244, 15), // "showContextMenu"
QT_MOC_LITERAL(23, 260, 3), // "pos"
QT_MOC_LITERAL(24, 264, 19) // "populateContextMenu"

    },
    "nc::gui::TextView\0contextMenuCreated\0"
    "\0QMenu*\0menu\0status\0message\0highlight\0"
    "std::vector<Range<int> >\0ranges\0"
    "ensureVisible\0moveCursor\0position\0"
    "saveAs\0zoomIn\0delta\0zoomOut\0setDocumentFont\0"
    "font\0selectFont\0updateExtraSelections\0"
    "updatePositionStatus\0showContextMenu\0"
    "pos\0populateContextMenu"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_nc__gui__TextView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  104,    2, 0x06 /* Public */,
       5,    1,  107,    2, 0x06 /* Public */,
       5,    0,  110,    2, 0x26 /* Public | MethodCloned */,

 // slots: name, argc, parameters, tag, flags
       7,    2,  111,    2, 0x0a /* Public */,
       7,    1,  116,    2, 0x2a /* Public | MethodCloned */,
      11,    2,  119,    2, 0x0a /* Public */,
      11,    1,  124,    2, 0x2a /* Public | MethodCloned */,
      13,    0,  127,    2, 0x0a /* Public */,
      14,    1,  128,    2, 0x0a /* Public */,
      14,    0,  131,    2, 0x2a /* Public | MethodCloned */,
      16,    1,  132,    2, 0x0a /* Public */,
      16,    0,  135,    2, 0x2a /* Public | MethodCloned */,
      17,    1,  136,    2, 0x0a /* Public */,
      19,    0,  139,    2, 0x0a /* Public */,
      20,    0,  140,    2, 0x08 /* Private */,
      21,    0,  141,    2, 0x08 /* Private */,
      22,    1,  142,    2, 0x08 /* Private */,
      24,    1,  145,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8, QMetaType::Bool,    9,   10,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   12,   10,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QFont,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   23,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void nc::gui::TextView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TextView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->contextMenuCreated((*reinterpret_cast< QMenu*(*)>(_a[1]))); break;
        case 1: _t->status((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->status(); break;
        case 3: _t->highlight((*reinterpret_cast< std::vector<Range<int> >(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->highlight((*reinterpret_cast< std::vector<Range<int> >(*)>(_a[1]))); break;
        case 5: _t->moveCursor((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: _t->moveCursor((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->saveAs(); break;
        case 8: _t->zoomIn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->zoomIn(); break;
        case 10: _t->zoomOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->zoomOut(); break;
        case 12: _t->setDocumentFont((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 13: _t->selectFont(); break;
        case 14: _t->updateExtraSelections(); break;
        case 15: _t->updatePositionStatus(); break;
        case 16: _t->showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 17: _t->populateContextMenu((*reinterpret_cast< QMenu*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TextView::*)(QMenu * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextView::contextMenuCreated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TextView::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextView::status)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject nc::gui::TextView::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_nc__gui__TextView.data,
    qt_meta_data_nc__gui__TextView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *nc::gui::TextView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *nc::gui::TextView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_nc__gui__TextView.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int nc::gui::TextView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void nc::gui::TextView::contextMenuCreated(QMenu * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void nc::gui::TextView::status(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
