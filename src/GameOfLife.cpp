#include "plugin.hpp"


struct GameOfLife : Module {
	enum ParamIds {
		MODINDEXATV_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		SENDIN_INPUT,
		VOCTIN_INPUT,
		FMIN_INPUT,
		MODINDEXIN_INPUT,
		CLOCK_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		DATACLOCKOUT_OUTPUT,
		DATAOUT_OUTPUT,
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		DATALIGHT_LIGHT,
		CLOCKLIGHT_LIGHT,
		AUDIOLIGHT_LIGHT,
		NUM_LIGHTS
	};

	GameOfLife() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(MODINDEXATV_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct GameOfLifeWidget : ModuleWidget {
	GameOfLifeWidget(GameOfLife* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/GameOfLife.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.837, 99.697)), module, GameOfLife::MODINDEXATV_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.858, 42.535)), module, GameOfLife::SENDIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.469, 71.639)), module, GameOfLife::VOCTIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(36.2, 71.639)), module, GameOfLife::FMIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.837, 110.81)), module, GameOfLife::MODINDEXIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.008, 110.81)), module, GameOfLife::CLOCK_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.273, 27.432)), module, GameOfLife::DATACLOCKOUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.2, 42.535)), module, GameOfLife::DATAOUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.941, 110.81)), module, GameOfLife::AUDIO_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(11.402, 27.432)), module, GameOfLife::DATALIGHT_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(19.008, 99.697)), module, GameOfLife::CLOCKLIGHT_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(35.941, 99.697)), module, GameOfLife::AUDIOLIGHT_LIGHT));

		// mm2px(Vec(110.0, 110.0))
		addChild(createWidget<Widget>(mm2px(Vec(49.288, 11.674))));
	}
};


Model* modelGameOfLife = createModel<GameOfLife, GameOfLifeWidget>("GameOfLife");