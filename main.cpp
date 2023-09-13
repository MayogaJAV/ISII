#include <iostream>
#include <string>
#include <vector>
#include <tuple>

int main() {
	std::string id, nombre, apellidos, ciudad, comida;
	
	std::cout << "Ingrese el ID: ";
	std::cin >> id;
	
	std::cout << "Ingrese el Nombre: ";
	std::cin.ignore(); // Ignorar el salto de línea anterior
	std::getline(std::cin, nombre);
	
	std::cout << "Ingrese los Apellidos: ";
	std::getline(std::cin, apellidos);
	
	std::cout << "Ingrese la Ciudad: ";
	std::getline(std::cin, ciudad);
	
	std::cout << "Ingrese la Comida: ";
	std::getline(std::cin, comida);
	
	// Construir la lista con los valores ingresados
	std::vector<std::tuple<int, int>> primera_parte = {
		std::make_tuple(id.size(), nombre.size()),
			std::make_tuple(apellidos.size(), ciudad.size()),
			std::make_tuple(comida.size(), 0)
	};
	
	std::vector<int> segunda_parte = { 1, 1, 1 };
	
	std::vector<std::string> tercera_parte = { id, nombre, apellidos, ciudad, comida };
	
	// Mostrar la lista resultante
	std::cout << "Lista: [";
	for (size_t i = 0; i < primera_parte.size(); ++i) {
		std::cout << "(" << std::get<0>(primera_parte[i]) << ", " << std::get<1>(primera_parte[i]) << ")";
		if (i != primera_parte.size() - 1) {
			std::cout << ", ";
		}
	}
	std::cout << "], [";
	for (size_t i = 0; i < segunda_parte.size(); ++i) {
		std::cout << segunda_parte[i];
		if (i != segunda_parte.size() - 1) {
			std::cout << ", ";
		}
	}
	std::cout << "], [";
	for (size_t i = 0; i < tercera_parte.size(); ++i) {
		std::cout << "\"" << tercera_parte[i] << "\"";
		if (i != tercera_parte.size() - 1) {
			std::cout << ", ";
		}
	}
	std::cout << "]" << std::endl;
	
	return 0;
}
