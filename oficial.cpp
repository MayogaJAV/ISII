
#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>
#include <iomanip>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;



struct Pagina {
	int numero;
	struct Pagina* siguiente;
};

void crearCarpetaBuffer() {
	const char* rutaCarpetaBuffer = "buffer";
	
	if (mkdir(rutaCarpetaBuffer) != 0) {
		std::cout << "No se pudo crear el directorio: " << std::endl;
		return;
	}
	
	std::cout << "Se ha creado el directorio correctamente." << std::endl;
	
	
	int numPaginas;
	printf("Ingrese el número de páginas a crear: ");
	scanf("%d", &numPaginas);
	
	struct Pagina* paginas = NULL;
	
	// Crear las páginas con los bloques correspondientes
	for (int i = 1; i <= numPaginas; ++i) {
		// Leer el contenido del bloque correspondiente
		char rutaArchivoBloque[256];
		snprintf(rutaArchivoBloque, sizeof(rutaArchivoBloque), "directorio/bloque%d.txt", i);
		
		FILE* archivoBloque = fopen(rutaArchivoBloque, "r");
		if (archivoBloque == NULL) {
			printf("No se pudo abrir el archivo del bloque %d.\n", i);
			continue;
		}
		
		// Crear la página
		struct Pagina* nuevaPagina = (struct Pagina*)malloc(sizeof(struct Pagina));
		nuevaPagina->numero = i;
		nuevaPagina->siguiente = NULL;
		
		// Leer el contenido del bloque y copiarlo en la página
		char contenidoBloque[256];
		char rutaArchivoPagina[256];
		snprintf(rutaArchivoPagina, sizeof(rutaArchivoPagina), "%s/Pagina%d.txt", rutaCarpetaBuffer, i);
		FILE* archivoPagina = fopen(rutaArchivoPagina, "w");
		
		if (archivoPagina == NULL) {
			printf("No se pudo crear el archivo de la página %d.\n", i);
			fclose(archivoBloque);
			free(nuevaPagina);
			continue;
		}
		
		while (fgets(contenidoBloque, sizeof(contenidoBloque), archivoBloque) != NULL) {
			fprintf(archivoPagina, "%s", contenidoBloque);
		}
		
		fclose(archivoBloque);
		fclose(archivoPagina);
		
		// Agregar la página a la lista enlazada
		if (paginas == NULL) {
			paginas = nuevaPagina;
		} else {
			struct Pagina* temp = paginas;
			while (temp->siguiente != NULL) {
				temp = temp->siguiente;
			}
			temp->siguiente = nuevaPagina;
		}
	}
	
	// Liberar la memoria de las páginas
	struct Pagina* temp = paginas;
	while (temp != NULL) {
		struct Pagina* siguiente = temp->siguiente;
		free(temp);
		temp = siguiente;
	}
	
	printf("Se han creado los archivos de las páginas correctamente.\n");
}

void eliminarLineaEnPagina(const char* rutaCarpetaBuffer, int numPagina, int numLinea) {
	// Obtener la ruta del archivo de página
	char rutaArchivoPagina[256];
	snprintf(rutaArchivoPagina, sizeof(rutaArchivoPagina), "%s/pagina%d.txt", rutaCarpetaBuffer, numPagina);
	
	// Abrir el archivo de página en modo lectura
	FILE* archivoPagina = fopen(rutaArchivoPagina, "r");
	if (archivoPagina == NULL) {
		printf("No se pudo abrir el archivo de la página %d.\n", numPagina);
		return;
	}
	
	// Crear un archivo temporal para escribir el contenido actualizado
	FILE* archivoTemporal = fopen("temporal.txt", "w");
	if (archivoTemporal == NULL) {
		printf("No se pudo crear el archivo temporal.\n");
		fclose(archivoPagina);
		return;
	}
	
	// Eliminar la línea y reemplazarla por espacios en blanco
	char linea[256];
	int numLineaActual = 1;
	while (fgets(linea, sizeof(linea), archivoPagina) != NULL) {
		if (numLineaActual != numLinea) {
			fputs(linea, archivoTemporal);
		} else {
			int longitudLinea = strlen(linea);
			for (int i = 0; i < longitudLinea; i++) {
				fputc(' ', archivoTemporal);
			}
			fputc('\n', archivoTemporal);
		}
		numLineaActual++;
	}
	
	fclose(archivoPagina);
	fclose(archivoTemporal);
	
	// Renombrar el archivo temporal como el archivo de página original
	if (remove(rutaArchivoPagina) != 0) {
		printf("No se pudo eliminar el archivo de la página %d.\n", numPagina);
		return;
	}
	
	if (rename("temporal.txt", rutaArchivoPagina) != 0) {
		printf("No se pudo renombrar el archivo temporal.\n");
		return;
	}
	
	char choice;
	std::cout << "¿Desea eliminar otro registro? (S/N): ";
	std::cin >> choice;
	
	if (choice == 'S' || choice == 's') {
		int numPagina, numLinea;
		
		printf("Ingrese el número de página: ");
		scanf("%d", &numPagina);
		
		printf("Ingrese el número de línea a eliminar: ");
		scanf("%d", &numLinea);
		
		eliminarLineaEnPagina(rutaCarpetaBuffer, numPagina, numLinea); // Llamada recursiva para eliminar otro registro
	}
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void buscarNombreEnPaginas() {
	const char* carpetaBuffer = "buffer";
	char nombre[256];
	printf("Ingrese el PassangerID a buscar: ");
	scanf("%s", nombre);
	
	int numPagina = 1;
	char rutaArchivoPagina[256];
	snprintf(rutaArchivoPagina, sizeof(rutaArchivoPagina), "%s/pagina%d.txt", carpetaBuffer, numPagina);
	
	FILE* archivoPagina;
	char linea[256];
	char* token;
	int columna = 0;
	bool nombreEncontrado = false;
	
	// Buscar en todas las páginas
	while ((archivoPagina = fopen(rutaArchivoPagina, "r")) != NULL) {
		// Leer cada línea de la página
		while (fgets(linea, sizeof(linea), archivoPagina) != NULL) {
			// Obtener cada columna separada por #
			token = strtok(linea, "#");
			columna = 0;
			
			// Buscar el nombre en cada columna
			while (token != NULL) {
				columna++;
				if (columna == 0 && strcmp(token, nombre) == 0) {
					nombreEncontrado = true;
					printf("Fila encontrada en la página %d:\n", numPagina);
					printf("%s\n", linea);
					break;
				}
				token = strtok(NULL, "#");
			}
			if (nombreEncontrado)
				break;
		}
		fclose(archivoPagina);
		if (nombreEncontrado)
			break;
		numPagina++;
		snprintf(rutaArchivoPagina, sizeof(rutaArchivoPagina), "%s/pagina%d.txt", carpetaBuffer, numPagina);
	}
	
	if (!nombreEncontrado) {
		printf("No se encontró el nombre especificado.\n");
	}
}




//Clase Disco
class Disco {
	
public:
	int numPlatos_;             // Numero de platos en el disco
	int numSuperficies_;        // Numero de superficies por plato
	int numPistas_;             // Numero de pistas por superficie
	int numSectores_;           // Numero de sectores por pista
	int capacidadSector_;       // Capacidad de cada sector en bytes
	int numDisco_;              // Numero del disco
	int numSectoresPista;		//Numero de sectores por pista
	
	
	Disco() {
		numDisco_ = 1;          // Se puede ajustar el numero de disco
	}
	
	// Función para crear un directorio
	int crearDirectorio(const std::string& path) {
		if (CreateDirectoryA(path.c_str(), NULL) == 0) {
			// Si CreateDirectoryA devuelve 0, hubo un error al crear el directorio
			DWORD error = GetLastError();
			if (error != ERROR_ALREADY_EXISTS) {
				// Si el error no es ERROR_ALREADY_EXISTS, significa que ocurrio un error distinto
				return -1; // Devuelve -1 para indicar un error
			}
		}
		return 0; // Devuelve 0 para indicar exito en la creación del directorio
	}
	
	
	
	
	// Función para crear los sectores a partir de un archivo
	void crearSectoresDesdeArchivo() {
		// Pedir el número de sectores por pista
		int numSectoresPista;
		std::cout << "Ingrese el número de sectores por pista: ";
		std::cin >> numSectoresPista;
		
		// Capacidad de cada sector en bytes
		int capacidadSector;
		int NumRegistro;
		std::cout << "Ingrese el numero de registros: ";
		std::cin >> NumRegistro;
		capacidadSector=200*NumRegistro;
		
		// Leer el archivo general
		std::ifstream archivoGeneral("FILE_OFICIAL.txt");
		if (!archivoGeneral) {
			std::cout << "No se pudo abrir el archivo general." << std::endl;
			return;
		}
		
		// Crear la carpeta "disco"
		if (crearDirectorio("disco") != 0) {
			std::cout << "Error al crear la carpeta disco." << std::endl;
			return;
		}
		
		// Pedir el número de platos
		int numPlatos;
		std::cout << "Ingrese el número de platos: ";
		std::cin >> numPlatos;
		
		// Pedir el número de superficies por plato
		int numSuperficies = 2;
		std::cout << "Número de superficies por plato: 2 ";
		
		// Pedir el número de pistas por superficie
		int numPistas;
		std::cout << "Ingrese el número de pistas por superficie: ";
		std::cin >> numPistas;
		
		// Obtener el número total de sectores
		int numSectoresTotal = numPlatos * numSuperficies * numPistas * numSectoresPista;
		
		// Calcular la capacidad del disco en bytes
		long long capacidadDisco = static_cast<long long>(numSectoresTotal) * capacidadSector;
		
		// Crear las carpetas de los sectores y escribir la información desde el archivo general
		int numSector = 1;
		std::string nombreArchivoSector;
		std::ofstream archivoSector;
		std::vector<char> buffer(capacidadSector);
		
		for (int numPlato = 1; numPlato <= numPlatos; numPlato++) {
			std::string rutaCarpetaPlato = "disco\\plato" + std::to_string(numPlato);
			if (crearDirectorio(rutaCarpetaPlato) != 0) {
				std::cout << "Error al crear la carpeta " << rutaCarpetaPlato << "." << std::endl;
				continue;
			}
			
			for (int numSuperficie = 1; numSuperficie <= numSuperficies; numSuperficie++) {
				std::string rutaCarpetaSuperficie = rutaCarpetaPlato + "\\superficie" + std::to_string(numSuperficie);
				if (crearDirectorio(rutaCarpetaSuperficie) != 0) {
					std::cout << "Error al crear la carpeta " << rutaCarpetaSuperficie << "." << std::endl;
					continue;
				}
				
				for (int numPista = 1; numPista <= numPistas; numPista++) {
					std::string rutaCarpetaPista = rutaCarpetaSuperficie + "\\pista" + std::to_string(numPista);
					if (crearDirectorio(rutaCarpetaPista) != 0) {
						std::cout << "Error al crear la carpeta " << rutaCarpetaPista << "." << std::endl;
						continue;
					}
					
					for (int numSectorPista = 1; numSectorPista <= numSectoresPista; numSectorPista++) {
						// Verificar si el archivo general se ha llenado
						if (archivoGeneral.eof()) {
							// Construir el nombre del archivo sector vacio
							nombreArchivoSector = rutaCarpetaPista + "\\sector" + std::to_string(numSector) + ".txt";
							
							// Crear el archivo sector vacio
							archivoSector.open(nombreArchivoSector);
							archivoSector.close();
						}
						else {
							// Leer la información desde el archivo general
							archivoGeneral.read(buffer.data(), capacidadSector);
							
							// Construir el nombre del archivo sector
							nombreArchivoSector = rutaCarpetaPista + "\\sector" + std::to_string(numSector) + ".txt";
							
							// Crear o abrir el archivo sector
							archivoSector.open(nombreArchivoSector);
							if (!archivoSector) {
								std::cout << "No se pudo crear el archivo " << nombreArchivoSector << "." << std::endl;
								break;
							}
							
							// Escribir la informacion en el archivo sector
							archivoSector.write(buffer.data(), capacidadSector);
							
							// Cerrar el archivo sector
							archivoSector.close();
						}
						
						// Incrementar el numero de sector
						numSector++;
					}
				}
			}
		}
		
		// Cerrar el archivo general
		archivoGeneral.close();
		
		// Verificar si quedan sectores por llenar y crear archivos vacios para los sectores restantes si es necesario
		int numSectoresFaltantes = numSectoresTotal - (numSector - 1);
		for (int i = 0; i < numSectoresFaltantes; i++) {
			// Construir el nombre del archivo sector vacio
			std::string rutaCarpetaPista = "disco\\plato1\\superficie1\\pista1";
			nombreArchivoSector = rutaCarpetaPista + "\\sector" + std::to_string(numSector) + ".txt";
			
			// Crear el archivo sector vacio
			archivoSector.open(nombreArchivoSector);
			archivoSector.close();
			
			// Incrementar el numero de sector
			numSector++;
		}
		
		std::cout << "Se han creado los archivos de sectores correctamente." << std::endl;
		std::cout << "Capacidad total del disco: " << capacidadDisco << " bytes." << std::endl;
	}
	bool esDirectorio(const std::string& ruta) {
		DWORD atributos = GetFileAttributesA(ruta.c_str());
		return (atributos != INVALID_FILE_ATTRIBUTES && (atributos & FILE_ATTRIBUTE_DIRECTORY));
	}
	
	
	
	void agregarUbicacionesSectoresRecursivo(const std::string& rutaCarpeta, std::ofstream& archivoUbicaciones) {
		DIR* directorio = opendir(rutaCarpeta.c_str());
		if (directorio == nullptr) {
			std::cout << "No se pudo abrir la carpeta '" << rutaCarpeta << "'." << std::endl;
			return;
		}
		
		struct dirent* entrada;
		while ((entrada = readdir(directorio)) != nullptr) {
			std::string nombreEntrada = entrada->d_name;
			if (nombreEntrada != "." && nombreEntrada != "..") {
				std::string rutaEntrada = rutaCarpeta + "/" + nombreEntrada;
				if (entrada->d_type == DT_REG && nombreEntrada.find(".txt") != std::string::npos) {
					archivoUbicaciones << rutaEntrada << std::endl;
				} else if (entrada->d_type == DT_DIR) {
					agregarUbicacionesSectoresRecursivo(rutaEntrada, archivoUbicaciones);
				}
			}
		}
		
		closedir(directorio);
	}
	
	void agregarUbicacionesSectores(const std::string& rutaCarpetaDisco, const std::string& rutaArchivoUbicaciones) {
		std::ofstream archivoUbicaciones(rutaArchivoUbicaciones);
		if (!archivoUbicaciones) {
			std::cout << "No se pudo abrir el archivo de ubicaciones." << std::endl;
			return;
		}
		
		agregarUbicacionesSectoresRecursivo(rutaCarpetaDisco, archivoUbicaciones);
		
		archivoUbicaciones.close();
		std::cout << "Se han agregado las ubicaciones de los sectores correctamente." << std::endl;
	}
	
	
	// Función para buscar el sector y mostrar su ubicación y contenido en consola
	void obtenerInformacionSector() {
		// Pedir el numero de sector
		int numSector;
		std::cout << "Ingrese el número del sector: ";
		std::cin >> numSector;
		
		// Calcular la ubicacion del sector en el disco
		int numPlatos, numSuperficies, numPistas, numSectoresPista;
		// Asigna los valores correspondientes según la estructura del disco
		// ...
		
		// Calcular la ubicacion del sector en el disco
		// Calcula el numero total de sectores por plato
		int sectorPorPlato = numSuperficies * numPistas * numSectoresPista;
		
		// Calcula el numero total de sectores por superficie
		int sectorPorSuperficie = numPistas * numSectoresPista;
		
		// Almacena el numero de sectores por pista
		int sectorPorPista = numSectoresPista;
		
		// Calcula el numero de plato correspondiente al numero de sector
		int numPlato = (numSector - 1) / sectorPorPlato + 1;
		
		// Calcula el numero de superficie correspondiente al numero de sector
		int numSuperficie = ((numSector - 1) % sectorPorPlato) / sectorPorSuperficie + 1;
		
		// Calcula el numero de pista correspondiente al número de sector
		int numPista = (((numSector - 1) % sectorPorPlato) % sectorPorSuperficie) / sectorPorPista + 1;
		std::cout << "El sector " << numSector << " se encuentra en:" << std::endl;
		std::cout << "Disco: " << numDisco_ << std::endl;
		std::cout << "Plato: " << numPlato << std::endl;
		std::cout << "Superficie: " << numSuperficie << std::endl;
		std::cout << "Pista: " << numPista << std::endl;
		std::cout << "Sector: " << numSector << std::endl;
		
		// Obtener la ruta del archivo del sector
		std::string rutaArchivo = "disco/plato" + std::to_string(numPlato) + "/superficie" + std::to_string(numSuperficie) + "/pista" + std::to_string(numPista) + "/sector" + std::to_string(numSector) + ".txt";
		
		// Abrir el archivo del sector
		std::ifstream archivoSector(rutaArchivo);
		if (!archivoSector) {
			std::cout << "No se pudo abrir el archivo del sector " << numSector << "." << std::endl;
			return;
		}
		
		std::cout << "Contenido del sector " << numSector << ":" << std::endl;
		
		std::string linea;
		while (std::getline(archivoSector, linea)) {
			std::cout << linea << std::endl;
		}
		archivoSector.close();
		
		char choice;
		std::cout << "¿Desea obtener informacion de otro sector? (S/N): ";
		std::cin >> choice;
		
		if (choice == 'S' || choice == 's') {
			obtenerInformacionSector();  // Llamada recursiva para obtener informacion de otro sector
		}
		
	}
	
	
};



//Clases de Directorio
class Directorio {
public:
	Directorio(const char* rutaDirectorio) : rutaDirectorio_(rutaDirectorio) {}
	
	void crearDirectorio() {
		// Crear el directorio
		if (mkdir(rutaDirectorio_) != 0) {
			std::cout << "No se pudo crear el directorio: " << rutaDirectorio_ << std::endl;
			return;
		}
		
		std::cout << "Se ha creado el directorio '" << rutaDirectorio_ << "' correctamente." << std::endl;
	}
	
#include <iostream>
#include <cstdio>
	
	void generarArchivosBloque(int numBloques) {
		// Abrir el archivo general
		FILE* archivoGeneral = fopen("ubicaciones_sectores.txt", "r");
		if (archivoGeneral == NULL) {
			std::cout << "No se pudo abrir el archivo general." << std::endl;
			return;
		}
		
		int Nums;
		std::cout << "Ingrese el numero de sectores por bloque: " << std::endl;
		std::cin >> Nums;
		
		// Leer y generar los archivos de cada bloque
		char rutaArchivo[256];
		for (int bloque = 1; bloque <= numBloques; ++bloque) {
			snprintf(rutaArchivo, sizeof(rutaArchivo), "%s/bloque%d.txt", rutaDirectorio_, bloque);
			
			// Crear el archivo del bloque
			FILE* archivoBloque = fopen(rutaArchivo, "w");
			if (archivoBloque == NULL) {
				std::cout << "No se pudo crear el archivo del bloque " << bloque << "." << std::endl;
				continue;
			}
			
			// Leer Nums rutas del archivo general y escribir en el archivo del bloque
			char ruta[300];
			for (int i = 0; i < Nums; ++i) {
				if (fscanf(archivoGeneral, "%255s", ruta) == 1) {
					// Abrir el archivo de la ruta especificada
					FILE* contenidoArchivo = fopen(ruta, "r");
					if (contenidoArchivo != NULL) {
						// Leer y escribir el contenido del archivo en el archivo del bloque
						char caracter;
						while ((caracter = fgetc(contenidoArchivo)) != EOF) {
							fputc(caracter, archivoBloque);
						}
						fclose(contenidoArchivo);
					} else {
						std::cout << "No se pudo abrir el archivo de la ruta especificada en la línea " << ruta << "." << std::endl;
					}
				} else {
					std::cout << "No hay suficientes rutas en el archivo general para el bloque " << bloque << "." << std::endl;
					break;
				}
			}
			
			fclose(archivoBloque);
		}
		
		fclose(archivoGeneral);
		std::cout << "Se han generado los archivos de los bloques correctamente." << std::endl;
	}
	
	
	void imprimirContenidoBloque() {
		std::cout << "Ingrese el número del bloque: ";
		int numeroBloque;
		std::cin >> numeroBloque;
		
		std::string rutaArchivo = "directorio/bloque" + std::to_string(numeroBloque) + ".txt";
		std::ifstream archivoBloque(rutaArchivo);
		if (!archivoBloque) {
			std::cout << "No se pudo abrir el archivo del bloque " << numeroBloque << "." << std::endl;
			return;
		}
		
		std::ostringstream contenidoStream;
		contenidoStream << archivoBloque.rdbuf();
		std::cout << "Contenido del bloque " << numeroBloque << ":" << std::endl;
		std::cout << contenidoStream.str() << std::endl;
		
		archivoBloque.close();
	}
	
	
	
private:
		const char* rutaDirectorio_;
};







void TITANIC_CONVERTIDO()
{
	const char* inputFile = "titanic.csv";
	const char* outputFile = "titanic.txt";
	const char delimiter = ',';
	
	std::ifstream inFile(inputFile);
	std::ofstream outFile(outputFile);
	
	if (!inFile.is_open()) {
		std::cerr << "Error al abrir el archivo de entrada." << std::endl;
		return;
	}
	
	if (!outFile.is_open()) {
		std::cerr << "Error al abrir el archivo de salida." << std::endl;
		inFile.close();
		return;
	}
	
	char line[1024];
	bool isFirstLine = true;
	while (inFile.getline(line, sizeof(line)))
	{
		if (isFirstLine) {
			isFirstLine = false;
			continue;
		}
		
		bool insideQuotes = false;
		char* pos = line;
		char* writePos = line;
		while (*pos != '\0')
		{
			if (*pos == '"') {
				insideQuotes = !insideQuotes;
			} else if (*pos == delimiter && !insideQuotes) {
				*writePos = '#';
				writePos++;
			} else if (*pos != '"') {
				*writePos = *pos;
				writePos++;
			}
			
			pos++;
		}
		
		*writePos = '\0';
		outFile << line << std::endl;
	}
	
	inFile.close();
	outFile.close();
	
}



void CREAR_FILE()
{
	const char* inputFile = "titanic.txt";
	const char* outputFile = "FILE_OFICIAL.txt";
	const int columnWidths[] = { 8, 8, 8, 90, 8, 8, 15, 18, 18, 8, 15, 1 };
	int numColumns = sizeof(columnWidths) / sizeof(columnWidths[0]);
	
	std::ifstream inFile(inputFile);
	std::ofstream outFile(outputFile);
	
	if (!inFile.is_open()) {
		std::cerr << "Error al abrir el archivo de entrada (titanic.txt)." << std::endl;
		return;
	}
	
	if (!outFile.is_open()) {
		std::cerr << "Error al abrir el archivo de salida (FILE_MEJORADO.txt)." << std::endl;
		inFile.close();
		return;
	}
	
	char line[1024];
	while (inFile.getline(line, sizeof(line)))
	{
		char* token = std::strtok(line, "#");
		int i = 0;
		
		while (token != nullptr && i < numColumns)
		{
			const char* columnValue = token;
			int columnWidth = columnWidths[i];
			size_t columnLength = std::strlen(columnValue);
			int difference = columnWidth - columnLength;
			
			outFile << columnValue;
			for (int j = 0; j < difference; j++) {
				outFile << ' ';
			}
			
			outFile << '#';
			
			token = std::strtok(nullptr, "#");
			i++;
		}
		outFile << std::endl;
	}
	
	inFile.close();
	outFile.close();
}


//imprimir por fila
void IMPRIMIR_FILA(const char* filename, int targetRow) {
	std::ifstream file(filename);
	if (!file) {
		std::cout << "No se pudo abrir el archivo." << std::endl;
		return;
	}
	
	char line[256]; // Tamaño máximo de línea
	int currentRow = 0;
	bool isTargetRow = false;
	
	while (file.getline(line, sizeof(line))) {
		currentRow++;
		
		if (currentRow == targetRow) {
			isTargetRow = true;
			for (int i = 0; line[i] != '\0'; i++) {
				if (line[i] != '#') {
					std::cout << line[i];
				}
			}
			std::cout << std::endl;
			break;
		}
	}
	
	if (!isTargetRow) {
		std::cout << "El numero de fila especificado excede el numero de filas en el archivo." << std::endl;
	}
}




//bit por fila 
void BIT_FILA(const char* filename, int rowNumber) {
	std::ifstream file(filename);
	
	if (!file) {
		std::cout << "No se pudo abrir el archivo." << std::endl;
		return;
	}
	
	char ch;
	int currentRow = 0;
	int bitCountWithHash = 0;
	int bitCountWithoutHash = 0;
	
	while (file.get(ch)) {
		if (ch == '\n') {
			currentRow++;
		}
		
		if (currentRow == rowNumber) {
			bitCountWithHash++;
			if (ch != '#') {
				bitCountWithoutHash++;
			}
		}
		
		if (currentRow > rowNumber) {
			break;
		}
	}
	
	file.close();
	
	if (currentRow < rowNumber) {
		std::cout << "El numero de fila especificado excede el numero de filas en el archivo." << std::endl;
		return;
	}
	
	std::cout << "El numero de bits por fila (con '#') es: " << bitCountWithHash << std::endl;
	std::cout << "El numero de bits por fila (sin '#') es: " << bitCountWithoutHash << std::endl;
	
}


// Funcion para calcular la cantidad de bytes y bits en un FILE
void BIT_FILE(const char* filename, unsigned long long& bytes, unsigned long long& bits) {
	bytes = 0;
	bits = 0;
	
	// Abrir el archivo en modo lectura
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cout << "Error al abrir el archivo." << std::endl;
		return;
	}
	
	// Leer el archivo lÃ­nea por lÃ­nea
	std::string line;
	while (std::getline(file, line)) {
		// Recorrer los caracteres de cada lÃ­nea
		for (char c : line) {
			// Excluir los caracteres "#"
			if (c != '#') {
				bytes++;
				bits += 8;
			}
		}
	}
	
	// Cerrar el archivo
	file.close();
}

//PAGINA




//menu
void MENU_PRINCIPAL() {
	std::cout << "=== Menú Principal ===" << std::endl;
	std::cout << "1. Bite por File" << std::endl;
	std::cout << "2. Bit por Fila" << std::endl;
	std::cout << "3. Imprimir registro" << std::endl;
	std::cout << "4. Disco y Directorio" << std::endl;
	std::cout << "5. Paginas" << std::endl;
	std::cout << "6. Salir" << std::endl;
}
//menu secundario
void SUB_MENU() {
	std::cout << "=== Submenú - Disco y Directorio ===" << std::endl;
	std::cout << "1. Creacion de Disco" << std::endl;
	std::cout << "2. Informacion Sector" << std::endl;
	std::cout << "3. Bloque" << std::endl;
	std::cout << "4. Volver al Menú Principal" << std::endl;
}

int main()
{
	TITANIC_CONVERTIDO();
	CREAR_FILE();
	int choice;
	int subChoice;
	bool inSubMenu = false;
	
	while (true) {
		if (inSubMenu) {
			SUB_MENU();
			std::cout << "Ingrese su elección: ";
			std::cin >> subChoice;
			
			switch (subChoice) {
			case 1:
			{
				int numSector;
				Disco disco;
				disco.crearSectoresDesdeArchivo();
				std::string rutaCarpetaDisco = "disco";
				std::string rutaArchivoUbicaciones = "ubicaciones_sectores.txt";
				disco.agregarUbicacionesSectores(rutaCarpetaDisco,rutaArchivoUbicaciones);
				
				
				break; 
			}
			case 2:
			{
				Disco disco;
				disco.obtenerInformacionSector();
				break;
			}
			case 3: {
				const char* rutaDirectorio = "directorio";
				int numBloques;
				std::cout<<"Ingrese el numero de bloques: "<<std::endl;
				std::cin>>numBloques;
				
				Directorio directorio(rutaDirectorio);
				directorio.crearDirectorio();
				directorio.generarArchivosBloque(numBloques);
				directorio.imprimirContenidoBloque();
				break;
				
			}
			case 4:
				inSubMenu = false;  // Salir del submenú y volver al menú principal
				break;
			default:
				std::cout << "Elección inválida. Por favor, intente nuevamente." << std::endl;
				break;
			}
		} else {
			MENU_PRINCIPAL();
			std::cout << "Ingrese su elección: ";
			std::cin >> choice;
			
			switch (choice) {
			case 1:
			{
				const char* filename = "FILE_OFICIAL.txt";
				unsigned long long bytes, bits;
				// Llamar a la funcion para calcular la cantidad de bytes y bits en el archivo de texto
				BIT_FILE(filename, bytes, bits);
				std::cout << "Cantidad de bytes en el archivo: " << bytes << " bytes." << std::endl;
				std::cout << "Cantidad de bits en el archivo: " << bits << " bits." << std::endl;
				
				break;}
			case 2:
			{
				int rowNumber; // Variable para almacenar el nUmero de fila especificado
				std::cout << "Ingrese el numero de registro: ";
				std::cin >> rowNumber;
				const char* filename = "FILE_OFICIAL.txt";
				BIT_FILA(filename, rowNumber);
				break; 
			}
			case 3:
				
			{
				int targetRow;
				std::cout << "Ingrese el numero de registro a imprimir: ";
				std::cin >>targetRow;
				const char* filename = "FILE_OFICIAL.txt";
				IMPRIMIR_FILA(filename, targetRow);
				break;
			}
			case 4:
				inSubMenu = true;  // Entrar al submenú
				break;
			case 5:{
				crearCarpetaBuffer();
				int numPagina, numLinea;
				
				printf("Ingrese el número de página: ");
				scanf("%d", &numPagina);
				
				printf("Ingrese el número de línea a eliminar: ");
				scanf("%d", &numLinea);
				
				char rutaCarpetaBuffer[256];
				snprintf(rutaCarpetaBuffer, sizeof(rutaCarpetaBuffer), "buffer");
				
				eliminarLineaEnPagina(rutaCarpetaBuffer, numPagina, numLinea);
				
				printf("Se ha eliminado la línea correctamente.\n");
				
				buscarNombreEnPaginas();
				
			}
			case 6:
					std::cout << "Saliendo del programa..." << std::endl;
					return 0;
			default:
				std::cout << "Elección inválida. Por favor, intente nuevamente." << std::endl;
				break;
			}
		}
	}
	
	
	
	return 0;
}

