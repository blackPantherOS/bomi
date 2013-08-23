#include "dialogs.hpp"
#include "ui_aboutdialog.h"
#include "info.hpp"
#include "widgets.hpp"
#include "global.hpp"
#include "playlist.hpp"
#include "info.hpp"
#include "appstate.hpp"

struct CheckDialog::Data {
	QCheckBox *check;
	QLabel *label;
	QDialogButtonBox *button;
	QDialogButtonBox::StandardButton clicked;
};

CheckDialog::CheckDialog(QWidget *parent, QDialogButtonBox::StandardButtons buttons)
: QDialog(parent), d(new Data) {
	d->check = new QCheckBox(this);
	d->label = new QLabel(this);
	d->button = new QDialogButtonBox(this);
	d->button->setCenterButtons(true);
	d->clicked = QDialogButtonBox::NoButton;

	auto vbox = new QVBoxLayout(this);
	vbox->addWidget(d->label);
	vbox->addWidget(d->check);
	auto hbox = new QHBoxLayout;
	hbox->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
	hbox->addWidget(d->button);
	hbox->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
	vbox->addLayout(hbox);
	connect(d->button, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
	setButtonBox(buttons);
}

CheckDialog::~CheckDialog() {
	delete d;
}

void CheckDialog::setLabelText(const QString &text) {
	d->label->setText(text);
}

void CheckDialog::setCheckBoxText(const QString &text) {
	d->check->setText(text);
}

void CheckDialog::setChecked(bool checked) {
	d->check->setChecked(checked);
}

bool CheckDialog::isChecked() const {
	return d->check->isChecked();
}

void CheckDialog::setButtonBox(QDialogButtonBox::StandardButtons buttons) {
	d->button->setStandardButtons(buttons);
}

int CheckDialog::exec() {
	QDialog::exec();
	return d->clicked;
}

void CheckDialog::onButtonClicked(QAbstractButton *button) {
	d->clicked = d->button->standardButton(button);
	accept();
}

/*******************************************************************************************/

struct GetShortcutDialog::Data {
	QLineEdit *edit = nullptr;
	QPushButton *begin = nullptr, *erase = nullptr, *ok = nullptr, *cancel = nullptr;
	int curIdx = 0;
	int codes[MaxKeyCount];
};

GetShortcutDialog::GetShortcutDialog(const QKeySequence &shortcut, QWidget *parent)
: QDialog(parent) {
	d = new Data;
	d->edit = new QLineEdit(this);
	d->edit->setReadOnly(true);
	d->begin = new QPushButton(tr("Get Shortcut"), this);
	d->begin->setFocusPolicy(Qt::NoFocus);
	d->begin->setCheckable(true);
	d->begin->setDefault(false);
	d->erase = new QPushButton(tr("Erase"), this);
	d->erase->setFocusPolicy(Qt::NoFocus);
	d->erase->setAutoDefault(false);
	d->ok = new QPushButton(tr("Ok"), this);
	d->ok->setFocusPolicy(Qt::NoFocus);
	d->cancel = new QPushButton(tr("Cancel"), this);
	d->cancel->setAutoDefault(false);

	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->addWidget(d->edit);

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->addWidget(d->begin);
	hbox->addWidget(d->erase);
	hbox->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	hbox->addWidget(d->ok);
	hbox->addWidget(d->cancel);

	vbox->addLayout(hbox);

	connect(d->begin, SIGNAL(toggled(bool)), this, SLOT(setGetting(bool)));
	connect(d->erase, SIGNAL(clicked()), this, SLOT(erase()));
	connect(d->ok, SIGNAL(clicked()), this, SLOT(accept()));
	connect(d->cancel, SIGNAL(clicked()), this, SLOT(reject()));
	d->edit->installEventFilter(this);
	d->begin->toggle();

	setShortcut(shortcut);
}

GetShortcutDialog::~GetShortcutDialog() {
	delete d;
}

void GetShortcutDialog::erase() {
	d->curIdx = 0;
	for (int i=0; i<MaxKeyCount; ++i)
		d->codes[i] = 0;
	d->edit->clear();
}

QKeySequence GetShortcutDialog::shortcut() const {
	return QKeySequence(d->codes[0], d->codes[1], d->codes[2], d->codes[3]);
}

void GetShortcutDialog::setShortcut(const QKeySequence &shortcut) {
	for (int i=0; i<MaxKeyCount; ++i)
		d->codes[i] = shortcut[i];
	d->edit->setText(shortcut.toString(QKeySequence::NativeText));
}

void GetShortcutDialog::setGetting(bool on) {
	if (on)
		erase();
}

bool GetShortcutDialog::eventFilter(QObject *obj, QEvent *event) {
	if (obj == d->edit && d->begin->isChecked() && event->type() == QEvent::KeyPress) {
		getShortcut(static_cast<QKeyEvent *>(event));
		return true;
	} else
		return QDialog::eventFilter(obj, event);
}

void GetShortcutDialog::keyPressEvent(QKeyEvent *event) {
	QDialog::keyPressEvent(event);
	if (d->begin->isChecked())
		getShortcut(event);
}

void GetShortcutDialog::getShortcut(QKeyEvent *event) {
	if (0 <= d->curIdx && d->curIdx < MaxKeyCount) {
		d->codes[d->curIdx] = event->key();
		if (!(event->modifiers() & Qt::KeypadModifier))
			d->codes[d->curIdx] += event->modifiers();
	}
}

void GetShortcutDialog::keyReleaseEvent(QKeyEvent *event) {
	QDialog::keyReleaseEvent(event);
	if (d->begin->isChecked() && d->codes[d->curIdx]) {
		d->edit->setText(shortcut().toString(QKeySequence::NativeText));
		++d->curIdx;
	}
}

/*************************************************************************/

EncodingFileDialog::EncodingFileDialog(QWidget *parent, const QString &caption
		, const QString &directory, const QString &filter, const QString &encoding)
: QFileDialog(parent, caption, directory, filter), combo(new EncodingComboBox(this)) {
	auto grid = qobject_cast<QGridLayout*>(layout());
	if (grid) {
		const int row = grid->rowCount();
		grid->addWidget(new QLabel("Encoding:", this), row, 0, 1, 1);
		grid->addWidget(combo, row, 1, 1, grid->columnCount()-1);
	}
	if (!encoding.isEmpty())
		setEncoding(encoding);
}

void EncodingFileDialog::setEncoding(const QString &encoding) {
	combo->setEncoding(encoding);
}

QString EncodingFileDialog::encoding() const {
	return combo->encoding();
}

QString EncodingFileDialog::getOpenFileName(QWidget *parent
		, const QString &caption, const QString &dir, const QString &filter, QString *enc) {
	const auto files = getOpenFileNames(parent, caption, dir, filter, enc, ExistingFile);
	return files.isEmpty() ? QString() : files[0];
}

QStringList EncodingFileDialog::getOpenFileNames(QWidget *parent
		, const QString &caption, const QString &dir, const QString &filter, QString *enc, FileMode mode) {
	EncodingFileDialog dlg(parent, caption, dir, filter);
	if (enc && !enc->isEmpty())
		dlg.setEncoding(*enc);
	dlg.setFileMode(mode);
	if (dlg.exec()) {
		if (enc)
			*enc = dlg.encoding();
		return dlg.selectedFiles();
	}
	return QStringList();
}

/******************************************************************************/

struct GetUrlDialog::Data {
	QLineEdit *url;
	EncodingComboBox *enc;
	QCompleter *c;
};

GetUrlDialog::GetUrlDialog(QWidget *parent)
: QDialog(parent), d(new Data) {
	const auto &as = AppState::get();
	d->c = new QCompleter(as.open_url_list, this);
	d->url = new QLineEdit(this);
	d->url->setCompleter(d->c);
	d->enc = new EncodingComboBox(this);
	d->enc->setEncoding(as.open_url_enc);

	auto vbox = new QVBoxLayout(this);
	auto hbox = new QHBoxLayout;
	hbox->addWidget(new QLabel(tr("URL"), this));
	hbox->addWidget(d->url);
	vbox->addLayout(hbox);
	hbox = new QHBoxLayout;
	hbox->addWidget(new QLabel(tr("Encoding for Playlist"), this));
	hbox->addWidget(d->enc);
	hbox->addWidget(makeButtonBox(this));
	vbox->addLayout(hbox);
}

GetUrlDialog::~GetUrlDialog() {
	delete d;
}

void GetUrlDialog::accept() {
	auto &as = AppState::get();
	const auto url = d->url->text().trimmed();
	const int idx = as.open_url_list.indexOf(url);
	if (idx >= 0)
		as.open_url_list.takeAt(idx);
	as.open_url_list.prepend(url);
	as.open_url_enc = d->enc->encoding();
	QDialog::accept();
}

QUrl GetUrlDialog::url() const {
	return QUrl(d->url->text().trimmed());
}

//bool GetUrlDialog::isPlaylist() const {
//	const QString suffix = QFileInfo(url().path()).suffix();
//	return suffix.compare("pls", Qt::CaseInsensitive) == 0;
//}

//Playlist GetUrlDialog::playlist() const {
//	if (!isPlaylist())
//		return Playlist();
//	Playlist list;
//	list.load(url(), d->enc->encoding());
//	return list;
//}

QString GetUrlDialog::encoding() const {
	return d->enc->encoding();
}

/******************************************************************************/

struct AboutDialog::Data {
	Ui::AboutDialog ui;
};

AboutDialog::AboutDialog(QWidget *parent)
: QDialog(parent), d(new Data) {
	d->ui.setupUi(this);
#define UI_LABEL_ARG(label, arg) d->ui.label->setText(d->ui.label->text().arg)
	UI_LABEL_ARG(version, arg(Info::version()));
	UI_LABEL_ARG(copyright, arg(QDate::currentDate().year()));
	UI_LABEL_ARG(contacts, arg("<a href=\"http://xylosper.net\">http://xylosper.net</a><br>")
		.arg("<a href=\"mailto:darklin20@gmail.com\">darklin20@gmail.com</a><br>")
		.arg("<a href=\"http://cmplayer.github.com\">http://cmplayer.github.com</a>"));
#undef UI_LABEL_ARG
	d->ui.license->setText(
		"This program is free software; "
		"you can redistribute it and/or modify it under the terms of "
		"the GNU General Public License as published by the Free Software Foundation; "
		"either version 2 of the License, or (at your option) any later version.<br><br>"

		"This program is distributed in the hope that it will be useful, "
		"but WITHOUT ANY WARRANTY; without even the implied warranty of "
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "
		"See the GNU General Public License for more details.<br><br>"

		"You should have received a copy of the GNU General Public License along with this program; "
		"if not, see <a href=\"http://www.gnu.org/licenses\">http://www.gnu.org/licenses</a>.<br><br>"

		"Exception:<br>"
		"libchardet made by JoungKyun.Kim is distributed under Mozilla Public License(MPL)."
	);
	connect(d->ui.view_gpl, SIGNAL(clicked()), this, SLOT(showFullLicense()));
	connect(d->ui.view_mpl, SIGNAL(clicked()), this, SLOT(showFullLicense()));

	setFixedHeight(420);
	setFixedWidth(width());
}

AboutDialog::~AboutDialog() {
	delete d;
}

void AboutDialog::showFullLicense() {
	QDialog dlg(this);
	auto text = new QTextBrowser(&dlg);
	auto close = new QPushButton(tr("Close"), &dlg);
	auto vbox = new QVBoxLayout(&dlg);
	vbox->addWidget(text);
	auto hbox = new QHBoxLayout;
	hbox->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
	hbox->addWidget(close);
	hbox->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
	vbox->addLayout(hbox);
	connect(close, SIGNAL(clicked()), &dlg, SLOT(accept()));

	const QString fileName(sender() == d->ui.view_mpl ? ":/mpl.html" : ":/gpl.html");
	QFile file(fileName);
	file.open(QFile::ReadOnly | QFile::Text);
	text->setHtml(QString::fromLatin1(file.readAll()));
	dlg.resize(500, 400);
	dlg.exec();
}


#include "ui_opendvddialog.h"
#include <QtCore/QFileInfo>

struct OpenDvdDialog::Data {
	Ui::OpenDVDDialog ui;
	QPushButton *ok;
};

OpenDvdDialog::OpenDvdDialog(QWidget *parent)
: QDialog(parent), d(new Data) {
	d->ui.setupUi(this);
	d->ok = d->ui.buttonBox->button(QDialogButtonBox::Ok);
	d->ok->setEnabled(false);
	connect(d->ui.device, SIGNAL(editTextChanged(QString)), this, SLOT(checkDevice(QString)));
}

OpenDvdDialog::~OpenDvdDialog() {
	delete d;
}

void OpenDvdDialog::setDevices(const QStringList &devices) {
	d->ui.device->clear();
	d->ui.device->addItems(devices);
}

void OpenDvdDialog::checkDevice(const QString &device) {
	const QFileInfo info(device);
	const bool exists = info.exists();
	d->ok->setEnabled(exists);
	if (exists)
		d->ui.available->setText(tr("Selected device is available."));
	else {
		d->ui.available->setText(_L("<font color='red'>") % tr("Selected device doesn't exists.") % _L("</font>"));
	}
}

QString OpenDvdDialog::device() const {
	return d->ui.device->currentText();
}

