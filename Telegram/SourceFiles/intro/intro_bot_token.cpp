/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "intro/intro_bot_token.h"

#include "config.h"
#include "lang/lang_keys.h"
#include "ui/widgets/fields/input_field.h"
#include "styles/style_intro.h"

namespace Intro {
namespace details {

BotTokenWidget::BotTokenWidget(
	QWidget *parent,
	not_null<Main::Account*> account,
	not_null<Data*> data)
: Step(parent, account, data)
, _token(this, st::introName, tr::lng_intro_bot_token_placeholder()) {
	_token->setInputMethodHints(
		Qt::ImhLatinOnly
		| Qt::ImhNoAutoUppercase
		| Qt::ImhNoPredictiveText
		| Qt::ImhSensitiveData);

	_token->submits(
	) | rpl::on_next([=] {
		submit();
	}, _token->lifetime());

	setErrorCentered(true);

	setTitleText(tr::lng_intro_bot_token_title());
	setDescriptionText(tr::lng_intro_bot_token_desc());
}

void BotTokenWidget::resizeEvent(QResizeEvent *e) {
	Step::resizeEvent(e);
	updateControlsGeometry();
}

void BotTokenWidget::updateControlsGeometry() {
	_token->moveToLeft(
		contentLeft(),
		contentTop() + st::introStepFieldTop);
}

void BotTokenWidget::setInnerFocus() {
	_token->setFocusFast();
}

void BotTokenWidget::activate() {
	Step::activate();
	_token->show();
	setInnerFocus();
}

void BotTokenWidget::cancelled() {
	api().request(base::take(_sentRequest)).cancel();
}

void BotTokenWidget::tokenSubmitDone(const MTPauth_Authorization &result) {
	finish(result);
}

void BotTokenWidget::tokenSubmitFail(const MTP::Error &error) {
	_sentRequest = 0;
	if (MTP::IsFloodError(error)) {
		showError(tr::lng_flood_error());
		_token->setFocus();
		return;
	}

	const auto &err = error.type();
	if (err == u"ACCESS_TOKEN_INVALID"_q
		|| err == u"ACCESS_TOKEN_EXPIRED"_q) {
		showError(tr::lng_intro_bot_token_invalid());
		_token->setFocus();
	} else if (!MTP::IgnoreError(error)) {
		showError(rpl::single(err));
		_token->setFocus();
	}
}

void BotTokenWidget::submit() {
	if (_sentRequest) {
		return;
	}
	const auto token = _token->getLastText().trimmed();
	if (token.isEmpty()) {
		_token->setFocus();
		return;
	}

	hideError();
	_sentRequest = api().request(MTPauth_ImportBotAuthorization(
		MTP_int(0),
		MTP_int(ApiId),
		MTP_string(ApiHash),
		MTP_string(token)
	)).done([=](const MTPauth_Authorization &result) {
		tokenSubmitDone(result);
	}).fail([=](const MTP::Error &error) {
		tokenSubmitFail(error);
	}).handleFloodErrors().send();
}

rpl::producer<QString> BotTokenWidget::nextButtonText() const {
	return tr::lng_intro_bot_token_login();
}

} // namespace details
} // namespace Intro
