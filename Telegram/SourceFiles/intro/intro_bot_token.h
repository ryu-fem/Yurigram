/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "intro/intro_step.h"

namespace Ui {
class InputField;
} // namespace Ui

namespace Intro {
namespace details {

class BotTokenWidget final : public Step {
public:
	BotTokenWidget(
		QWidget *parent,
		not_null<Main::Account*> account,
		not_null<Data*> data);

	void setInnerFocus() override;
	void activate() override;
	void cancelled() override;
	void submit() override;
	rpl::producer<QString> nextButtonText() const override;

	bool hasBack() const override {
		return true;
	}

protected:
	void resizeEvent(QResizeEvent *e) override;

private:
	void updateControlsGeometry();
	void tokenSubmitDone(const MTPauth_Authorization &result);
	void tokenSubmitFail(const MTP::Error &error);

	object_ptr<Ui::InputField> _token;
	mtpRequestId _sentRequest = 0;

};

} // namespace details
} // namespace Intro
