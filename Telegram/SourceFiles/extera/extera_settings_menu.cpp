/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#include <ui/boxes/single_choice_box.h>

#include "extera/extera_settings.h"
#include "extera/extera_lang.h"
#include "extera/extera_settings_menu.h"
#include "lang_auto.h"
#include "mainwindow.h"
#include "settings/settings_common.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/checkbox.h"
#include "boxes/connection_box.h"
#include "platform/platform_specific.h"
#include "window/window_session_controller.h"
#include "lang/lang_instance.h"
#include "core/application.h"
#include "storage/localstorage.h"
#include "data/data_session.h"
#include "main/main_session.h"
#include "styles/style_settings.h"
#include "apiwrap.h"
#include "api/api_blocked_peers.h"
#include "ui/widgets/continuous_sliders.h"

#define SettingsMenuJsonSwitch(LangKey, Option) AddButton( \
	container, \
	rktr(#LangKey), \
	st::settingsButton \
)->toggleOn( \
	rpl::single(::ExteraSettings::JsonSettings::GetBool(#Option)) \
)->toggledValue( \
) | rpl::filter([](bool enabled) { \
	return (enabled != ::ExteraSettings::JsonSettings::GetBool(#Option)); \
}) | rpl::start_with_next([](bool enabled) { \
	::ExteraSettings::JsonSettings::Set(#Option, enabled); \
	::ExteraSettings::JsonSettings::Write(); \
}, container->lifetime());

namespace Settings {

    rpl::producer<QString> Extera::title() {
        return rktr("etg_settings_extera");
    }

    Extera::Extera(
            QWidget *parent,
            not_null<Window::SessionController *> controller)
            : Section(parent) {
        setupContent(controller);
    }

    void Extera::SetupChats(not_null<Ui::VerticalLayout *> container) {
        AddSubsectionTitle(container, rktr("etg_settings_chats"));

    	const auto recentStickersLabel = container->add(
			object_ptr<Ui::LabelSimple>(
				container,
				st::settingsAudioVolumeLabel),
			st::settingsAudioVolumeLabelPadding);
    	const auto recentStickersSlider = container->add(
			object_ptr<Ui::MediaSlider>(
				container,
				st::settingsAudioVolumeSlider),
			st::settingsAudioVolumeSliderPadding);
    	const auto updateRecentStickersLabel = [=](int value) {
    		const auto count = QString::number(value);
    		recentStickersLabel->setText(ktr("etg_settings_recent_stickers_limit", value, { "count", count }));
    	};
    	const auto updateRecentStickers = [=](int value) {
    		updateRecentStickersLabel(value);
    		::ExteraSettings::JsonSettings::Set("recent_stickers_limit", value);
    		::ExteraSettings::JsonSettings::Write();
    	};
    	recentStickersSlider->resize(st::settingsAudioVolumeSlider.seekSize);
    	recentStickersSlider->setPseudoDiscrete(
			101,
			[](int val) { return val; },
			::ExteraSettings::JsonSettings::GetInt("recent_stickers_limit"),
			updateRecentStickers);
    	updateRecentStickersLabel(::ExteraSettings::JsonSettings::GetInt("recent_stickers_limit"));
    	
        const auto stickerHeightLabel = container->add(
		    object_ptr<Ui::LabelSimple>(
			    container,
			    st::settingsAudioVolumeLabel),
		    st::settingsAudioVolumeLabelPadding);
	    const auto stickerHeightSlider = container->add(
		    object_ptr<Ui::MediaSlider>(
			    container,
			    st::settingsAudioVolumeSlider),
		    st::settingsAudioVolumeSliderPadding);
	    const auto updateStickerHeightLabel = [=](int value) {
		    const auto pixels = QString::number(value);
		    stickerHeightLabel->setText(ktr("etg_settings_sticker_height", { "pixels", pixels }));
	    };
	    const auto updateStickerHeight = [=](int value) {
		    updateStickerHeightLabel(value);
		    ::ExteraSettings::JsonSettings::Set("sticker_height", value);
		    ::ExteraSettings::JsonSettings::Write();
	    };
	    stickerHeightSlider->resize(st::settingsAudioVolumeSlider.seekSize);
	    stickerHeightSlider->setPseudoDiscrete(
		    193,
		    [](int val) { return val + 64; },
		    ::ExteraSettings::JsonSettings::GetInt("sticker_height"),
		    updateStickerHeight);
	    updateStickerHeightLabel(::ExteraSettings::JsonSettings::GetInt("sticker_height"));
    	
        container->add(
		object_ptr<Ui::Checkbox>(
			container,
			ktr("etg_settings_sticker_scale_both"),
			::ExteraSettings::JsonSettings::GetBool("sticker_scale_both"),
			st::settingsCheckbox),
		st::settingsCheckboxPadding
	    )->checkedChanges(
	        ) | rpl::filter([](bool checked) {
		        return (checked != ::ExteraSettings::JsonSettings::GetBool("sticker_scale_both"));
	        }) | rpl::start_with_next([](bool checked) {
		        ::ExteraSettings::JsonSettings::Set("sticker_scale_both", checked);
		        ::ExteraSettings::JsonSettings::Write();
	        }, container->lifetime());

		SettingsMenuJsonSwitch(etg_settings_chat_id, show_ids);
    }

	void Extera::SetupOther(not_null<Ui::VerticalLayout *> container) {
		AddSubsectionTitle(container, rktr("etg_settings_other"));

		SettingsMenuJsonSwitch(etg_settings_show_phone_number, show_phone_in_settings);
	}

    void Extera::SetupExteraSettings(not_null<Ui::VerticalLayout *> container, not_null<Window::SessionController *> controller) {
        AddSkip(container);
        SetupChats(container);

		AddSkip(container);
		SetupOther(container);
    }

    void Extera::setupContent(not_null<Window::SessionController *> controller) {
        const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

        SetupExteraSettings(content, controller);

        Ui::ResizeFitChild(this, content);
    }
} // namespace Settings
