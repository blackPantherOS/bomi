#ifndef ABOUTDIALOG_HPP
#define ABOUTDIALOG_HPP

class AboutDialog : public QDialog {
    Q_OBJECT
public:
    AboutDialog(QWidget *parent = 0);
    ~AboutDialog();
private:
    struct Data;
    Data *d = nullptr;
};

#endif // ABOUTDIALOG_HPP
