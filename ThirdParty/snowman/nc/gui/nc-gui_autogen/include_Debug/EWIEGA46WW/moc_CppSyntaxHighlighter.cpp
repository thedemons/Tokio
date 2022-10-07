/****************************************************************************
** Meta object code from reading C++ file 'CppSyntaxHighlighter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../CppSyntaxHighlighter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CppSyntaxHighlighter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_nc__gui__CxxFormatting_t {
    QByteArrayData data[10];
    char stringdata0[160];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_nc__gui__CxxFormatting_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_nc__gui__CxxFormatting_t qt_meta_stringdata_nc__gui__CxxFormatting = {
    {
QT_MOC_LITERAL(0, 0, 22), // "nc::gui::CxxFormatting"
QT_MOC_LITERAL(1, 23, 9), // "textColor"
QT_MOC_LITERAL(2, 33, 22), // "singleLineCommentColor"
QT_MOC_LITERAL(3, 56, 25), // "multiLineLineCommentColor"
QT_MOC_LITERAL(4, 82, 12), // "keywordColor"
QT_MOC_LITERAL(5, 95, 13), // "operatorColor"
QT_MOC_LITERAL(6, 109, 11), // "numberColor"
QT_MOC_LITERAL(7, 121, 10), // "macroColor"
QT_MOC_LITERAL(8, 132, 11), // "stringColor"
QT_MOC_LITERAL(9, 144, 15) // "escapeCharColor"

    },
    "nc::gui::CxxFormatting\0textColor\0"
    "singleLineCommentColor\0multiLineLineCommentColor\0"
    "keywordColor\0operatorColor\0numberColor\0"
    "macroColor\0stringColor\0escapeCharColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_nc__gui__CxxFormatting[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       9,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::QColor, 0x00085103,
       2, QMetaType::QColor, 0x00085103,
       3, QMetaType::QColor, 0x00085003,
       4, QMetaType::QColor, 0x00085103,
       5, QMetaType::QColor, 0x00085103,
       6, QMetaType::QColor, 0x00085103,
       7, QMetaType::QColor, 0x00085103,
       8, QMetaType::QColor, 0x00085103,
       9, QMetaType::QColor, 0x00085103,

       0        // eod
};

void nc::gui::CxxFormatting::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<CxxFormatting *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = _t->textColor(); break;
        case 1: *reinterpret_cast< QColor*>(_v) = _t->singleLineCommentColor(); break;
        case 2: *reinterpret_cast< QColor*>(_v) = _t->multiLineCommentColor(); break;
        case 3: *reinterpret_cast< QColor*>(_v) = _t->keywordColor(); break;
        case 4: *reinterpret_cast< QColor*>(_v) = _t->operatorColor(); break;
        case 5: *reinterpret_cast< QColor*>(_v) = _t->numberColor(); break;
        case 6: *reinterpret_cast< QColor*>(_v) = _t->macroColor(); break;
        case 7: *reinterpret_cast< QColor*>(_v) = _t->stringColor(); break;
        case 8: *reinterpret_cast< QColor*>(_v) = _t->escapeCharColor(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<CxxFormatting *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setTextColor(*reinterpret_cast< QColor*>(_v)); break;
        case 1: _t->setSingleLineCommentColor(*reinterpret_cast< QColor*>(_v)); break;
        case 2: _t->setMultiLineCommentColor(*reinterpret_cast< QColor*>(_v)); break;
        case 3: _t->setKeywordColor(*reinterpret_cast< QColor*>(_v)); break;
        case 4: _t->setOperatorColor(*reinterpret_cast< QColor*>(_v)); break;
        case 5: _t->setNumberColor(*reinterpret_cast< QColor*>(_v)); break;
        case 6: _t->setMacroColor(*reinterpret_cast< QColor*>(_v)); break;
        case 7: _t->setStringColor(*reinterpret_cast< QColor*>(_v)); break;
        case 8: _t->setEscapeCharColor(*reinterpret_cast< QColor*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject nc::gui::CxxFormatting::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_nc__gui__CxxFormatting.data,
    qt_meta_data_nc__gui__CxxFormatting,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *nc::gui::CxxFormatting::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *nc::gui::CxxFormatting::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_nc__gui__CxxFormatting.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int nc::gui::CxxFormatting::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 9;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_nc__gui__CppSyntaxHighlighter_t {
    QByteArrayData data[1];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_nc__gui__CppSyntaxHighlighter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_nc__gui__CppSyntaxHighlighter_t qt_meta_stringdata_nc__gui__CppSyntaxHighlighter = {
    {
QT_MOC_LITERAL(0, 0, 29) // "nc::gui::CppSyntaxHighlighter"

    },
    "nc::gui::CppSyntaxHighlighter"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_nc__gui__CppSyntaxHighlighter[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void nc::gui::CppSyntaxHighlighter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject nc::gui::CppSyntaxHighlighter::staticMetaObject = { {
    QMetaObject::SuperData::link<QSyntaxHighlighter::staticMetaObject>(),
    qt_meta_stringdata_nc__gui__CppSyntaxHighlighter.data,
    qt_meta_data_nc__gui__CppSyntaxHighlighter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *nc::gui::CppSyntaxHighlighter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *nc::gui::CppSyntaxHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_nc__gui__CppSyntaxHighlighter.stringdata0))
        return static_cast<void*>(this);
    return QSyntaxHighlighter::qt_metacast(_clname);
}

int nc::gui::CppSyntaxHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSyntaxHighlighter::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
