#ifndef PARSETHREAD_H
#define PARSETHREAD_H

#include <QObject>
#include <QThread>
#include <QMap>
#include <QVector>
#include <QStringList>
#include <QDomDocument>
#include <QDateTime>

///Object created by ScriptTable::InsertWidget.
typedef struct
{
    int row;
    int column;
    QString className;
}TableWidgetSubObject;


///Contains the data of a function which returns a object (e.g. String).
typedef struct
{
    bool isArray;
    QString functionName;
    QString resultType;

}FunctionWithResultObject;

typedef enum
{
    ENTRY_TYPE_VAR = 0,
    ENTRY_TYPE_FUNCTION,
    ENTRY_TYPE_CLASS,
    ENTRY_TYPE_CLASS_FUNCTION,
    ENTRY_TYPE_CLASS_THIS_FUNCTION,
    ENTRY_TYPE_CLASS_VAR,
    ENTRY_TYPE_MAP,
    ENTRY_TYPE_MAP_VAR,
    ENTRY_TYPE_MAP_FUNC,
    ENTRY_TYPE_PARSE_ERROR,
    ENTRY_TYPE_PROTOTYPE_FUNC,
    ENTRY_TYPE_FILE

}EntryType;

///A parsed entry.
typedef struct ParsedEntry ParsedEntry;
struct ParsedEntry
{
    int line;
    int column;
    QString name;
    QString completeName;
    EntryType type;
    QStringList params;
    int tabIndex;
    QVector<ParsedEntry> subElements;
};

class ParseThread : public QThread
{
    Q_OBJECT
public:
    explicit ParseThread(QObject *parent = 0);

protected:
    ///The thread main function.
    void run();

signals:

    ///Is emitted if the parsing is finished.
    ///Note: autoCompletionApiFiles contains the auto-completion entries for all parsed files.
    void parsingFinishedSignal(QMap<QString, QStringList> autoCompletionEntries, QMap<QString, QStringList> autoCompletionApiFiles,
                               QMap<QString, QStringList> parsedUiObjects, QMap<int,QVector<ParsedEntry>> parsedEntries, bool doneParsing,
                               bool parseOnlyUIFiles);

public slots:

    ///Parses the current text. Emits parsingFinishedSignal if the parsing is finished.
    void parseSlot(QMap<QString, QString> loadedUiFiles, QMap<int, QString> loadedScripts, QMap<int, QString> loadedScriptsIndex,
                   bool loadedFileChanged, bool parseOnlyUIFiles);

private:

    ///Returns all functions and gloabl variables in the loaded script files.
    QMap<int,QVector<ParsedEntry>> getAllFunctionsAndGlobalVariables(QMap<int, QString> loadedScripts);

    ///Searches objects which are returned by a ScriptTableWidget.
    void searchSingleTableSubWidgets(QString objectName, QVector<TableWidgetSubObject> subObjects, QStringList& lines);

    ///Searches all ScriptTableWidget::insertWidgets calls of a specific ScriptTableWidget object.
    void parseTableWidetInsert(const QString objectName, QStringList lines);

    ///Searches a single object type.
    void searchSingleType(QString className, QString searchString, QStringList& lines, bool isArray=false,
                                        bool withOutDotsAndBracked= false, bool replaceExistingEntry=false, bool matchExact = false);

    ///Searches all functions which return objects to for a specific object.
    void seachAllFunctionForSpecificObject(QStringList& lines, QString &currentText,
                                                        QVector<FunctionWithResultObject>& functions, QString objectName);
    ///Searches all dynamically created objects created by custom objects (like ScriptTimer).
    void checkDocumentForCustomDynamicObjects(QStringList& lines, QStringList &linesWithBrackets , QString &currentText, int passNumber);

    ///Searches all dynamically created objects created by standard objects (like String).
    void checkDocumentForStandardDynamicObjects(QStringList& lines, QString &currentText, QStringList &linesWithBrackets, int passNumber);

    ///Checks if in the current document user interface files are loaded.
    void checkDocumentForUiFiles(QString& currentText, QString activeDocument);

    ///Adds on obect to the auto-completion list.
    void addObjectToAutoCompletionList(QString& objectName, QString& className, bool isGuiElement, bool isArray=false, bool replaceExistingEntry=false);

    ///Parses a widget list from a user interface file (auto-completion).
    void parseWidgetList(QString uiFileName, QDomElement& docElem, bool parseActions);

    ///Parses an user interface file (auto-completion).
    void parseUiFile(QString uiFileName, QString fileContent);

    ///Removes all unnecessary characters (e.g. comments).
    void removeAllUnnecessaryCharacters(QString& currentText);

    ///Removes all square brackets and all between them.
    void removeAllBetweenSquareBrackets(QString& currentText);

    ///Parses a single line from an api file and adds functions which return objects to m_functionsWithResultObjects.
    void parseSingleLineForFunctionsWithResultObjects(QString singleEntry);

    ///Contains the auto-completion entries for all parsed api files.
    QMap<QString, QStringList> m_autoCompletionApiFiles;

    ///Contains all auto-completion entries (all but for the parsed api files).
    QMap<QString, QStringList> m_autoCompletionEntries;

    ///True if an entry hass been added to m_autoCompletionApiFiles.
    bool m_objectAddedToCompletionList;

    ///Contains all Objects which can create other objects.
    QMap<QString, QString> m_creatorObjects;

    ///Contains all String Objects.
    QVector<QString> m_stringList;

    ///Contains all Objects with a unknown type.
    QVector<QString> m_unknownTypeObjects;

    ///Contains all Array Objects.
    QVector<QString> m_arrayList;

    ///Contains all ScriptTableEidget Objects.
    QVector<QString> m_tableWidgets;

    ///Contains all objects created by ScriptTable::InsertWidget.
    QMap<QString, QVector<TableWidgetSubObject>> m_tableWidgetObjects;

    ///Contains all parsed ui files.
    QStringList m_parsedUiFiles;

    ///Contains the last changed time of all parsed ui files which where loaded from file.
    QMap<QString, QDateTime> m_parsedUiFilesFromFile;

    ///Contains the data of a function which returns a object (e.g. String).
    QMap<QString, QVector<FunctionWithResultObject>> m_functionsWithResultObjects;

    ///Contains the parsed UI objects.
    QMap<QString, QStringList> m_parsedUiObjects;

};

#endif // PARSETHREAD_H
