/****************************************************************************
** Meta object code from reading C++ file 'SearchWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../SearchWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SearchWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_nc__gui__SearchWidget_t {
    QByteArrayData data[9];
    char stringdata0[135];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_nc__gui__SearchWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_nc__gui__SearchWidget_t qt_meta_stringdata_nc__gui__SearchWidget = {
    {
QT_MOC_LITERAL(0, 0, 21), // "nc::gui::SearchWidget"
QT_MOC_LITERAL(1, 22, 8), // "activate"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 10), // "deactivate"
QT_MOC_LITERAL(4, 43, 8), // "findNext"
QT_MOC_LITERAL(5, 52, 12), // "findPrevious"
QT_MOC_LITERAL(6, 65, 25), // "scheduleIncrementalSearch"
QT_MOC_LITERAL(7, 91, 24), // "performIncrementalSearch"
QT_MOC_LITERAL(8, 116, 18) // "rememberCompletion"

    },
    "nc::gui::SearchWidget\0activate\0\0"
    "deactivate\0findNext\0findPrevious\0"
    "scheduleIncrementalSearch\0"
    "performIncrementalSearch\0rememberCompletion"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_nc__gui__SearchWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a /* Public */,
       3,    0,   50,    2, 0x0a /* Public */,
       4,    0,   51,    2, 0x0a /* Public */,
       5,    0,   52,    2, 0x0a /* Public */,
       6,    0,   53,    2, 0x08 /* Private */,
       7,    0,   54,    2, 0x08 /* Private */,
       8,    0,   55,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void nc::gui::SearchWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SearchWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->activate(); break;
        case 1: _t->deactivate(); break;
        case 2: _t->findNext(); break;
        case 3: _t->findPrevious(); break;
        case 4: _t->scheduleIncrementalSearch(); break;
        case 5: _t->performIncrementalSearch(); break;
        case 6: _t->rememberCompletion(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject nc::gui::SearchWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_nc__gui__SearchWidget.data,
    qt_meta_data_nc__gui__SearchWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *nc::gui::SearchWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *nc::gui::SearchWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_nc__gui__SearchWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int nc::gui::SearchWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
