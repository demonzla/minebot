#ifndef SCRIPTRUNNER_H
#define SCRIPTRUNNER_H

#include "Game.h"

#include <QObject>
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QUrl>
#include <QTextStream>
#include <QTimer>

class ScriptRunner : public QObject
{
    Q_OBJECT
public:
    ScriptRunner(QUrl url, QString script_file, bool debug, bool headless, QObject * parent = NULL);

    // start the engine. returns success
    bool go();

public slots:
    // methods that JavaScript uses
    void print(QString text);
    void debug(QString line);
    void chat(QString message);
    void exit();

public:
    // functions that JavaScript uses
    static QScriptValue username(QScriptContext * context, QScriptEngine * engine);
    static QScriptValue itemStackHeight(QScriptContext * context, QScriptEngine * engine);

    static QScriptValue setTimeout(QScriptContext * context, QScriptEngine * engine);
    static QScriptValue clearTimeout(QScriptContext * context, QScriptEngine * engine);
    static QScriptValue setInterval(QScriptContext * context, QScriptEngine * engine);
    static QScriptValue clearInterval(QScriptContext * context, QScriptEngine * engine);


private:
    QUrl m_url;
    QString m_script_filename;
    bool m_debug;
    bool m_headless;

    QScriptEngine * m_engine;
    QScriptValue m_bot;

    Game * m_game;

    bool m_exiting;

    QTextStream m_stderr;
    QTextStream m_stdout;

    struct TimedFunction {
        int id;
        bool repeat;
        QScriptValue this_ref;
        QScriptValue function;
        TimedFunction() {}
        TimedFunction(int id, bool repeat, QScriptValue this_ref, QScriptValue function) :
            id(id),
            repeat(repeat),
            this_ref(this_ref),
            function(function) {}
    };

    QHash<int, QTimer *> m_timer_ptrs;
    QHash<QTimer *, TimedFunction> m_script_timers;
    int m_timer_count;
private:
    void callBotMethod(QString method_name, const QScriptValueList & args = QScriptValueList());
    bool argCount(QScriptContext *context, int arg_count);
    int nextTimerId();

    int setTimeout(QScriptValue func, int ms, QScriptValue this_obj, bool repeat);

private slots:

    void movePlayerPosition(Server::EntityPosition position);
    void handleChunkUpdated(const Int3D &start, const Int3D &size);
    void handlePlayerHealthUpdated();
    void handlePlayerDied();
    void handleChatReceived(QString username, QString message);
    void handleLoginStatusUpdated(Server::LoginStatus status);

    void dispatchTimeout();
};

#endif // SCRIPTRUNNER_H
