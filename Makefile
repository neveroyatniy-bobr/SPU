EXECUTOR_PREF = executor/
TRANSLATOR_PREF = translator/
BUILD_PREF = build/
PROGRAM = program.exe

.PHONY: All, BuildExecutor, BuildTranslator, CleanExecutor, CleanTranslator

All:

BuildExecutor :	
	@make -C $(EXECUTOR_PREF)

BuildTranslator :
	@make -C $(TRANSLATOR_PREF)

CleanExecutor :
	@rm -rf $(EXECUTOR_PREF)$(BUILD_PREF)

CleanTranslator :
	@rm -rf $(TRANSLATOR_PREF)$(BUILD_PREF)