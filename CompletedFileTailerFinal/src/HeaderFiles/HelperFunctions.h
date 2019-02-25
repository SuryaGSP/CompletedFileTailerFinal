#pragma once
#include "JSONProcessor.h"
void IntializeThreadPoolInFileProcessor();
void processFiles(std::vector<JSONProcessor::fileNamePatternWithDirStruct> &fileNameStructvec);
ThreadSafeQueue<std::string>* getModifiedFileQueueObject();