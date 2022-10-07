/****************************************************************************
** Meta object code from reading C++ file 'GotoLineWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../GotoLineWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GotoLineWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_nc__gui__GotoLineWidget_t {
    QByteArrayData data[8];
    char stringdata0[90];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_nc__gui__GotoLineWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_nc__gui__GotoLineWidget_t qt_meta_stringdata_nc__gui__GotoLineWidget = {
    {
QT_MOC_LITERAL(0, 0, 23), // "nc::gui::GotoLineWidget"
QT_MOC_LITERAL(1, 24, 8), // "activate"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 10), // "deactivate"
QT_MOC_LITERAL(4, 45, 2), // "go"
QT_MOC_LITERAL(5, 48, 14), // "indicateStatus"
QT_MOC_LITERAL(6, 63, 7), // "success"
QT_MOC_LITERAL(7, 71, 18) // "rememberCompletion"

    },
    "nc::gui::GotoLineWidget\0activate\0\0"
    "deactivate\0go\0indicateStatus\0success\0"
    "rememberCompletion"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_nc__gui__GotoLineWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    0,   45,    2, 0x0a /* Public */,
       4,    0,   46,    2, 0x08 /* Private */,
       5,    1,   47,    2, 0x08 /* Private */,
       5,    0,   50,    2, 0x28 /* Private | MethodCloned */,
       7,    0,   51,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void nc::gui::GotoLineWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GotoLineWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->activate(); break;
        case 1: _t->deactivate(); break;
        case 2: _t->go(); break;
        case 3: _t->indicateStatus((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->indicateStatus(); break;
        case 5: _t->rememberCompletion(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject nc::gui::GotoLineWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_nc__gui__GotoLineWidget.data,
    qt_meta_data_nc__gui__GotoLineWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *nc::gui::GotoLineWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *nc::gui::GotoLineWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_nc__gui__GotoLineWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int nc::gui::GotoLineWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
