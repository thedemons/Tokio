/****************************************************************************
** Meta object code from reading C++ file 'TreeView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../TreeView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TreeView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_nc__gui__TreeView_t {
    QByteArrayData data[15];
    char stringdata0[148];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_nc__gui__TreeView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_nc__gui__TreeView_t qt_meta_stringdata_nc__gui__TreeView = {
    {
QT_MOC_LITERAL(0, 0, 17), // "nc::gui::TreeView"
QT_MOC_LITERAL(1, 18, 18), // "contextMenuCreated"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 6), // "QMenu*"
QT_MOC_LITERAL(4, 45, 4), // "menu"
QT_MOC_LITERAL(5, 50, 4), // "copy"
QT_MOC_LITERAL(6, 55, 6), // "zoomIn"
QT_MOC_LITERAL(7, 62, 5), // "delta"
QT_MOC_LITERAL(8, 68, 7), // "zoomOut"
QT_MOC_LITERAL(9, 76, 15), // "setDocumentFont"
QT_MOC_LITERAL(10, 92, 4), // "font"
QT_MOC_LITERAL(11, 97, 10), // "selectFont"
QT_MOC_LITERAL(12, 108, 15), // "showContextMenu"
QT_MOC_LITERAL(13, 124, 3), // "pos"
QT_MOC_LITERAL(14, 128, 19) // "populateContextMenu"

    },
    "nc::gui::TreeView\0contextMenuCreated\0"
    "\0QMenu*\0menu\0copy\0zoomIn\0delta\0zoomOut\0"
    "setDocumentFont\0font\0selectFont\0"
    "showContextMenu\0pos\0populateContextMenu"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_nc__gui__TreeView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   67,    2, 0x0a /* Public */,
       6,    1,   68,    2, 0x0a /* Public */,
       6,    0,   71,    2, 0x2a /* Public | MethodCloned */,
       8,    1,   72,    2, 0x0a /* Public */,
       8,    0,   75,    2, 0x2a /* Public | MethodCloned */,
       9,    1,   76,    2, 0x0a /* Public */,
      11,    0,   79,    2, 0x0a /* Public */,
      12,    1,   80,    2, 0x08 /* Private */,
      14,    1,   83,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QFont,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   13,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void nc::gui::TreeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TreeView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->contextMenuCreated((*reinterpret_cast< QMenu*(*)>(_a[1]))); break;
        case 1: _t->copy(); break;
        case 2: _t->zoomIn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->zoomIn(); break;
        case 4: _t->zoomOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->zoomOut(); break;
        case 6: _t->setDocumentFont((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 7: _t->selectFont(); break;
        case 8: _t->showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 9: _t->populateContextMenu((*reinterpret_cast< QMenu*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TreeView::*)(QMenu * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TreeView::contextMenuCreated)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject nc::gui::TreeView::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_nc__gui__TreeView.data,
    qt_meta_data_nc__gui__TreeView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *nc::gui::TreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *nc::gui::TreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_nc__gui__TreeView.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int nc::gui::TreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void nc::gui::TreeView::contextMenuCreated(QMenu * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
