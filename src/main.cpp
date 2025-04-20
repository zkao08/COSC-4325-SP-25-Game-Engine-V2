#include <memory>

#include "Application.h"

int main() {
	std::unique_ptr<Application> app = std::make_unique<Application>();

	return app->Execute();
}