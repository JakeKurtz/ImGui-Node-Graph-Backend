#pragma once
#include "log.h"

extern bool GLFW_INIT;

extern unsigned int id_counter;

extern Logger logger;

static unsigned int gen_id()
{
	return id_counter++;
}