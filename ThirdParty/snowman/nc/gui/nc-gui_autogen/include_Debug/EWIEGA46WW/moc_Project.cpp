/****************************************************************************
** Meta object code from reading C++ file 'Project.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Project.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Project.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_nc__gui__Project_t {
    QByteArrayData data[10];
    char stringdata0[126];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_nc__gui__Project_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_nc__gui__Project_t qt_meta_stringdata_nc__gui__Project = {
    {
QT_MOC_LITERAL(0, 0, 16), // "nc::gui::Project"
QT_MOC_LITERAL(1, 17, 11), // "nameChanged"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 12), // "imageChanged"
QT_MOC_LITERAL(4, 43, 19), // "instructionsChanged"
QT_MOC_LITERAL(5, 63, 11), // "treeChanged"
QT_MOC_LITERAL(6, 75, 11), // "disassemble"
QT_MOC_LITERAL(7, 87, 9), // "decompile"
QT_MOC_LITERAL(8, 97, 9), // "cancelAll"
QT_MOC_LITERAL(9, 107, 18) // "updateInstructions"

    },
    "nc::gui::Project\0nameChanged\0\0"
    "imageChanged\0instructionsChanged\0"
    "treeChanged\0disassemble\0decompile\0"
    "cancelAll\0updateInstructions"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_nc__gui__Project[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,
       4,    0,   56,    2, 0x06 /* Public */,
       5,    0,   57,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   58,    2, 0x0a /* Public */,
       7,    0,   59,    2, 0x0a /* Public */,
       8,    0,   60,    2, 0x0a /* Public */,
       9,    0,   61,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void nc::gui::Project::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Project *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->nameChanged(); break;
        case 1: _t->imageChanged(); break;
        case 2: _t->instructionsChanged(); break;
        case 3: _t->treeChanged(); break;
        case 4: _t->disassemble(); break;
        case 5: _t->decompile(); break;
        case 6: _t->cancelAll(); break;
        case 7: _t->updateInstructions(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Project::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Project::nameChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Project::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Project::imageChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Project::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Project::instructionsChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Project::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Project::treeChanged)) {
                *result = 3;
                return;
            }
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject nc::gui::Project::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_nc__gui__Project.data,
    qt_meta_data_nc__gui__Project,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *nc::gui::Project::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *nc::gui::Project::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_nc__gui__Project.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int nc::gui::Project::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void nc::gui::Project::nameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void nc::gui::Project::imageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void nc::gui::Project::instructionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void nc::gui::Project::treeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
