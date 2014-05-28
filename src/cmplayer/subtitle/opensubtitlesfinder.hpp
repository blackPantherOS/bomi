#ifndef OPENSUBTITLESFINDER_HPP
#define OPENSUBTITLESFINDER_HPP

class Mrl;

struct SubtitleLink {
    QString language, fileName, date;
    QUrl url;
};

class OpenSubtitlesFinder : public QObject {
    Q_OBJECT
public:
    enum State {
        Unavailable = 1, Error = 16, Connecting = 8, Available = 2, Finding = 4
    };
    OpenSubtitlesFinder(QObject *parent = nullptr);
    ~OpenSubtitlesFinder();
    auto find(const Mrl &mrl) -> bool;
    auto state() const -> State;
    auto isAvailable() const -> bool { return state() == Available; }
    auto error() const -> QString;
signals:
    void stateChanged();
    void found(const QList<SubtitleLink> &links);
private:
    struct Data;
    Data *d;
};

#endif // OPENSUBTITLESFINDER_HPP
