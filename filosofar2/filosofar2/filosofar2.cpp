#include <iostream>
#include <windows.h>
#include "filosofar2.h"

#define ANCHO 80
#define ALTO 25

struct variables_globales {

	int (*FI2_inicio)(int, unsigned long long, struct DatosSimulaciOn*, int const*) = nullptr;
	int (*FI2_inicioFilOsofo)(int) = nullptr;
	int (*FI2_pausaAndar)(void) = nullptr;
	int (*FI2_puedoAndar)(void) = nullptr;
	int (*FI2_andar)(void) = nullptr;
	int (*FI2_entrarAlComedor)(int) = nullptr;
	int (*FI2_cogerTenedor)(int) = nullptr;
	int (*FI2_comer)(void) = nullptr;
	int (*FI2_dejarTenedor)(int) = nullptr;
	int (*FI2_entrarAlTemplo)(int) = nullptr;
	int (*FI2_meditar)(void) = nullptr;
	int (*FI2_finFilOsofo)(void) = nullptr;
	int (*FI2_fin)(void) = nullptr;
	void (*pon_error)(char*) = nullptr;
	int (*FI2_aDOndeVoySiAndo)(int*, int*) = nullptr;

	HINSTANCE lib = nullptr;
	int numVue = 0;
	int numFil = 0;
	int ret = 0;

	int numFilIni = 0;

	int BUF_SIZE_COMEDOR = sizeof(int) * 8;
};

variables_globales* vg;
DatosSimulaciOn* dat_sim;
DWORD WINAPI funciOnFilOsofo(LPVOID parAmetro);

HANDLE coords[ANCHO][ALTO];
HANDLE tene[8];
HANDLE contIni;

CRITICAL_SECTION csP;

typedef struct {
	int cruzando;
	int direccion;
} Puente;


int main(int argc, char* argv[])
{

	vg = new variables_globales;
	dat_sim = new DatosSimulaciOn;
	HANDLE hijos[MAXFILOSOFOS];
	unsigned long long clave = 28545168465992;

	int dE[] = { 0, // Cigoto,
			  0, // PasilloSalidaTemplo,
			  0, // CaminoTemploPuente,
			  0, // PuenteHaciaComedor,
			  0, // CaminoPuenteComedor,
			  0, // PasilloEntradaComedor,
			  0, // Antesala,
			  0, // EntradaComedor,
			  0, // SillaComedor,
			  0, // SalidaComedor,
			  0, // PasilloSalidaComedor,
			  0, // ExplanadaComedorPuente,
			  0, // PuenteHaciaTemplo,
			  0, // CaminoPuenteTemplo,
			  0, // PasilloEntradaTemplo,
			  0, // EntradaTemplo,
			  0, // SitioTemplo,
			  0, // SalidaTemplo,
			  0  // Fin
	};


	///////////////////////////////////////////////////////////// LIBRERIA Y CARGA DE DLL

	vg->lib = LoadLibrary(TEXT("filosofar2.dll"));
	if (vg->lib == NULL) {
		PERROR("Error al cargar libreria filosofar2.dll");
		fflush(stderr);
		return -1;
	}

	vg->FI2_inicio = (int(*)(int, unsigned long long, struct DatosSimulaciOn*, int const*))GetProcAddress(vg->lib, "FI2_inicio");
	vg->FI2_inicioFilOsofo = (int(*)(int))GetProcAddress(vg->lib, "FI2_inicioFilOsofo");
	vg->FI2_pausaAndar = (int(*)())GetProcAddress(vg->lib, "FI2_pausaAndar");
	vg->FI2_puedoAndar = (int(*)())GetProcAddress(vg->lib, "FI2_puedoAndar");
	vg->FI2_andar = (int(*)())GetProcAddress(vg->lib, "FI2_andar");
	vg->FI2_entrarAlComedor = (int(*)(int))GetProcAddress(vg->lib, "FI2_entrarAlComedor");
	vg->FI2_cogerTenedor = (int(*)(int))GetProcAddress(vg->lib, "FI2_cogerTenedor");
	vg->FI2_comer = (int(*)())GetProcAddress(vg->lib, "FI2_comer");
	vg->FI2_dejarTenedor = (int(*)(int))GetProcAddress(vg->lib, "FI2_dejarTenedor");
	vg->FI2_entrarAlTemplo = (int(*)(int))GetProcAddress(vg->lib, "FI2_entrarAlTemplo");
	vg->FI2_meditar = (int(*)())GetProcAddress(vg->lib, "FI2_meditar");
	vg->FI2_finFilOsofo = (int(*)())GetProcAddress(vg->lib, "FI2_finFilOsofo");
	vg->FI2_fin = (int(*)())GetProcAddress(vg->lib, "FI2_fin");
	vg->FI2_aDOndeVoySiAndo = (int(*)(int*, int*))GetProcAddress(vg->lib, "FI2_aDOndeVoySiAndo");

	if (vg->FI2_inicio == NULL ||
		vg->FI2_inicioFilOsofo == NULL ||
		vg->FI2_pausaAndar == NULL ||
		vg->FI2_puedoAndar == NULL ||
		vg->FI2_andar == NULL ||
		vg->FI2_entrarAlComedor == NULL ||
		vg->FI2_cogerTenedor == NULL ||
		vg->FI2_comer == NULL ||
		vg->FI2_dejarTenedor == NULL ||
		vg->FI2_entrarAlTemplo == NULL ||
		vg->FI2_meditar == NULL ||
		vg->FI2_finFilOsofo == NULL ||
		vg->FI2_fin == NULL ||
		vg->FI2_aDOndeVoySiAndo == NULL)
	{
		PERROR("Error en la carga de funciones.");
		exit(100);
	}

	///////////////////////////////////////////////////////////// LIBRERIA Y CARGA DE DLL








	///////////////////////////////////////////////////////////// ARGUMENTOS 

	if (argc != 4) {
		fprintf(stderr, "Uso: %s <num-filosofos> <num-vueltas> <lentidud-ejecucion>\n", argv[0]);
		fprintf(stderr, "<num-filosofos>: Numero de filosofos que se van a ejecutar - (1 - %d)\n", MAXFILOSOFOS);
		fprintf(stderr, "<num-vueltas>: Numero de vueltas que cada filosofo debe dar\n");
		fprintf(stderr, "<lentidud-ejecucion>: Lentitud de ejecucion de la simulacion (0 - 10) \n");
		fflush(stderr);
		exit(100);
	}

	vg->numFil = atoi(argv[1]);
	if (vg->numFil < 1 || vg->numFil > MAXFILOSOFOS) {
		fprintf(stderr, "El numero de filosofos debe estar entre 1 y %d\n", MAXFILOSOFOS);
		fflush(stderr);
		exit(100);
	}
	vg->numVue = atoi(argv[2]);
	if (vg->numVue <= 0) {
		fprintf(stderr, "El numero de vueltas debe ser mayor a 0\n");
		fflush(stderr);
		exit(100);
	}
	vg->ret = atoi(argv[3]);
	if (vg->ret < 0) {
		fprintf(stderr, "La lentitud de ejecucion debe ser mayor o igual a 0\n");
		fflush(stderr);
		exit(100);
	}

	dat_sim->maxFilOsofosEnPuente = 4;
	dat_sim->maxUnaDirecciOnPuente = 4;
	dat_sim->nTenedores = 8;
	dat_sim->sitiosTemplo = 14;

	///////////////////////////////////////////////////////////// ARGUMENTOS 

	HANDLE mutexTemplo = CreateMutex(NULL, FALSE, "MUTTEMPLO");


	///////////////////////////////////////////////////////////// SINCRONIZACION DE INICIO

	for (int x = 0; x < ANCHO; x++) {
		for (int y = 0; y < ALTO; y++) {
			coords[x][y] = CreateMutex(NULL, FALSE, NULL);
			if (!coords[x][y]) {
				PERROR("Error en la creacion de casillas");
				exit(100);
			}
		}
	}

	contIni = CreateMutex(NULL, FALSE, NULL);
	if (!contIni) {
		PERROR("Error en la creacion del contador de inicio");
		exit(100);
	}

	HANDLE iniciar = CreateEvent(NULL, TRUE, FALSE, "INICIAR");
	if (!iniciar) {
		PERROR("Error en la creacion del evento de inicio");
		exit(100);
	}

	///////////////////////////////////////////////////////////// SINCRONIZACION DE INICIO




	///////////////////////////////////////////////////////////// SINCRONIZACION DE PUENTE

	HANDLE mutPue = CreateMutex(NULL, FALSE, "MUTPUE");
	if (!mutPue) {
		PERROR("Error en la creacion del mutex para el acceso a la memoria del puente");
		exit(100);
	}

	HANDLE cruzarIzq = CreateEvent(NULL, TRUE, FALSE, "CRUZARIZQ");
	if (!cruzarIzq) {
		PERROR("Error en la creacion del evento de cruzarPuente hacia la izquierda");
		exit(100);
	}

	HANDLE cruzarDer = CreateEvent(NULL, TRUE, FALSE, "CRUZARDER");
	if (!cruzarDer) {
		PERROR("Error en la creacion del evento de cruzarPuente hacia la derecha");
		exit(100);
	}

	HANDLE semPeso = CreateSemaphore(NULL,
		dat_sim->maxFilOsofosEnPuente,
		dat_sim->maxFilOsofosEnPuente,
		"SEMPESO");
	if (!semPeso) {
		PERROR("Error en la creacion del semaforo del peso del puente.");
		exit(100);
	}

	///////////////////////////////////////////////////////////// SINCRONIZACION DE PUENTE




	///////////////////////////////////////////////////////////// SINCRONIZACION DE COMEDOR

	HANDLE accMemCom = CreateMutex(NULL, FALSE, "MUTACCCOM");
	if (!accMemCom) {
		PERROR("Error en la creacion del mutex para el acceso a la memoria del comedor");
		exit(100);
	}

	for (int i = 0; i < dat_sim->nTenedores; i++) {
		tene[i] = CreateMutex(NULL, FALSE, NULL);
		if (!tene[i]) {
			PERROR("Error en la creacion de tenedores");
			exit(100);
		}
	}

	HANDLE semAnte = CreateSemaphore(NULL, 4, 4, "SEMANTE");
	if (!semAnte) {
		PERROR("Error en la creacion del semaforo de la antesala.");
		exit(100);
	}

	HANDLE semCome = CreateSemaphore(NULL, 7, 7, "SEMCOME");
	if (!semCome) {
		PERROR("Error en la creacion del semaforo del acceso al comedor.");
		exit(100);
	}

	///////////////////////////////////////////////////////////// SINCRONIZACION DE COMEDOR





	///////////////////////////////////////////////////////////// MEMORIA COMPARTIDA

	HANDLE memCom = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, vg->BUF_SIZE_COMEDOR, "MEMCOME");
	if (memCom == NULL) {
		PERROR("Error en la solicitud de memoria de los puestos de comedor.");
		exit(100);
	}
	LPCH comBase = (LPCH)MapViewOfFile(memCom, FILE_MAP_ALL_ACCESS, 0, 0, vg->BUF_SIZE_COMEDOR);
	if (comBase == NULL) {
		PERROR("Error en la asignacion del puntero a memoria del comedor.");
		exit(100);
	}
	LPCH com = comBase;
	for (int i = 0; i < 8; i++) {
		int val = 1;
		CopyMemory(com, &val, sizeof(int));
		com = com + 4;
	}

	HANDLE memPuen = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(Puente), "MEMPUEN");
	if (memPuen == NULL) {
		PERROR("Error en la solicitud de memoria de los puestos de comedor.");
		exit(100);
	}


	HANDLE memTemplo = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(char) * MAXSITIOSTEMPLO * 4, "MEMTEMPLO");
	if (memTemplo == NULL) {
		PERROR("Error en la solicitud de memoria de los puestos del templo.");
		exit(100);
	}

	char * baseTemplo = (char*)MapViewOfFile(memTemplo, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(char) * MAXSITIOSTEMPLO * 4);
	if (baseTemplo == NULL) {
		PERROR("Error en la asignacion del puntero a memoria del templo.");
		exit(100);
	}

	for (int i = 0; i < dat_sim->sitiosTemplo; i++) {
		baseTemplo[i * 4] = 1; 
	}
	UnmapViewOfFile(baseTemplo);

	///////////////////////////////////////////////////////////// MEMORIA COMPARTIDA







	///////////////////////////////////////////////////////////// INICIO DEL PADRE + CREACION DE PROCESOS HIJO. ->

	vg->FI2_inicio(vg->ret, clave, dat_sim, dE);

	for (int i = 0; i < vg->numFil; i++) {
		hijos[i] = CreateThread(NULL, 0, funciOnFilOsofo, LPVOID(i), 0, NULL);
		if (hijos[i] == NULL) {
			PERROR("Error al crear los hijos\r\n");
			exit(100);
		}
	}

	///////////////////////////////////////////////////////////// INICIO DEL PADRE + CREACION DE PROCESOS HIJO. <-



	///////////////////////////////////////////////////////////// PROCESO DE ESPERA. ->

	WaitForMultipleObjects(vg->numFil, hijos, TRUE, INFINITE);

	///////////////////////////////////////////////////////////// PROCESO DE ESPERA. <-



	///////////////////////////////////////////////////////////// LIBERACION DE RECURSOS ->

	for (int i = 0; i < vg->numFil; i++) {
		if (!CloseHandle(hijos[i])) {
			PERROR("Error al terminar los hilos de los hijos.\r\n");
			exit(100);
		}
	}

	for (int x = 0; x < ANCHO; x++) {
		for (int y = 0; y < ALTO; y++) {
			if (coords[x][y] != NULL) {
				if (!CloseHandle(coords[x][y])) {
					PERROR("Error en la eliminacion de casillas.");
					exit(100);
				}
			}
			else {
				PERROR("Error en la eliminacion de casillas (No han sido creadas correctamente).");
				exit(100);
			}
		}
	}

	if (!CloseHandle(iniciar)) {
		PERROR("Error en la eliminacion del evento iniciar.");
		exit(100);
	}

	if (!CloseHandle(cruzarIzq)) {
		PERROR("Error en la eliminacion del evento cruzarPuente.");
		exit(100);
	}

	if (!CloseHandle(cruzarDer)) {
		PERROR("Error en la eliminacion del evento cruzarPuente.");
		exit(100);
	}

	if (!CloseHandle(contIni)) {
		PERROR("Error en la eliminacion del mutex contIni.");
		exit(100);
	}

	if (!CloseHandle(accMemCom)) {
		PERROR("Error en la eliminacion del mutex accMemCom.");
		exit(100);
	}

	if (!CloseHandle(com)) {
		PERROR("Error en la eliminacion de la memoria compartida del comedor.");
		exit(100);
	}

	if (!FreeLibrary(vg->lib)) {
		PERROR("Error en la liberacion de la libreria");
		exit(100);
	}


	///////////////////////////////////////////////////////////// LIBERACION DE RECURSOS <-


	return 0;

}










///////////////////////////////////////////////////////////// THREAD FILOSOFO ->

DWORD WINAPI funciOnFilOsofo(LPVOID parAmetro) {

	int vueltas = 0;
	int nextZona = CAMPO;
	int id = (int)parAmetro;

	int x, y;
	int x0 = -1, y0 = -1;

	///////////////////////////////////////////////////////////// SINCRONIZACION DE INICIO ->

	HANDLE iniciar = OpenEvent(EVENT_ALL_ACCESS, FALSE, "INICIAR");
	if (iniciar == NULL) {
		PERROR("Error al inicializar iniciar");
		exit(100);
	}

	///////////////////////////////////////////////////////////// SINCRONIZACION DE INICIO <-




	///////////////////////////////////////////////////////////// SINCRONIZACION DE PUENTE ->

	HANDLE cruzarIzq = OpenEvent(EVENT_ALL_ACCESS, FALSE, "CRUZARIZQ");
	if (cruzarIzq == NULL) {
		PERROR("Error al inicializar cruzarDer");
		exit(100);
	}

	HANDLE cruzarDer = OpenEvent(EVENT_ALL_ACCESS, FALSE, "CRUZARDER");
	if (cruzarDer == NULL) {
		PERROR("Error al inicializar cruzarIzq");
		exit(100);
	}

	HANDLE semPeso = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SEMPESO");
	if (semPeso == NULL) {
		PERROR("Error al inicializar semPeso");
		exit(100);
	}

	HANDLE mutPue = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "MUTPUE");
	if (mutPue == NULL) {
		PERROR("Error al inicializar MUTEX del comedor");
		exit(100);
	}


	///////////////////////////////////////////////////////////// SINCRONIZACION DE PUENTE <-



	///////////////////////////////////////////////////////////// SINCRONIZACION DE COMEDOR ->


	HANDLE semAnte = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SEMANTE");
	if (semAnte == NULL) {
		PERROR("Error al inicializar semAnte");
		exit(100);
	}
	HANDLE semCome = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SEMCOME");
	if (semCome == NULL) {
		PERROR("Error al inicializar semCome");
		exit(100);
	}
	HANDLE accMemCom = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "MUTACCCOM");
	if (accMemCom == NULL) {
		PERROR("Error al inicializar MUTEX del comedor");
		exit(100);
	}

	///////////////////////////////////////////////////////////// SINCRONIZACION DE COMEDOR <-

	///////////////////////////////////////////////////////////// SINCRONIZACION DE TEMPLO ->

	HANDLE mutexTemplo = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "MUTTEMPLO");
	if (mutexTemplo == NULL) {
		PERROR("Error al inicializar MUTEX del templo");
		exit(100);
	}

	///////////////////////////////////////////////////////////// SINCRONIZACION DE TEMPLO <-


	///////////////////////////////////////////////////////////// MEMORIA COMPARTIDA ->

	HANDLE memCome = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MEMCOME");
	if (memCome == NULL) {
		PERROR("Error al inicializar memCome");
		exit(100);
	}

	HANDLE memPuen = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MEMPUEN");
	if (memPuen == NULL) {
		PERROR("Error al inicializar memCome");
		exit(100);
	}


	HANDLE memTemplo = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MEMTEMPLO");
	if (memTemplo == NULL) {
		PERROR("Error al inicializar memTemplo");
		exit(100);
	}


	///////////////////////////////////////////////////////////// MEMORIA COMPARTIDA <-

	///////////////////////////////////////////////////////////// INICIO DE PROCESOS ->

	vg->FI2_inicioFilOsofo(id);
	vg->FI2_aDOndeVoySiAndo(&x, &y);
	if (x0 == -1 && y0 == -1) {
		x0 = x - 1;
		y0 = y;
		WaitForSingleObject(coords[x0][y0], INFINITE);
	}

	WaitForSingleObject(contIni, INFINITE);
	vg->numFilIni++;
	if (vg->numFilIni == vg->numFil) {
		SetEvent(iniciar);
	}
	ReleaseMutex(contIni);

	WaitForSingleObject(iniciar, INFINITE);

	///////////////////////////////////////////////////////////// INICIO DE PROCESOS <-



	///////////////////////////////////////////////////////////// COMIENZO DEL BUCLE ->

	while (vueltas < vg->numVue) {

		///////////////////////////////////////////////////////////// ANDAR MIENTRAS LA SIGUIENTE ZONA SEA CAMPO ->

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona == CAMPO);


		///////////////////////////////////////////////////////////// QUEDARSE BLOQUEADO SIN CONSUMO DE CPU HASTA QUE SE DEN LAS CONDICIONES PARA CRUZAR EL PUENTE HACIA LA IZQUIERDA ->

		InitializeCriticalSection(&csP);
		BOOL entro = FALSE;
		Puente* refP = (Puente*)MapViewOfFile(memPuen, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Puente));
		if (refP == NULL) {
			PERROR("Error al abrir la memoria del puente.");
			exit(100);
		}

		while (!entro) {
			EnterCriticalSection(&csP);
			if (refP->cruzando == 0) {
				refP->direccion = 0;
				SetEvent(cruzarIzq);
				ResetEvent(cruzarDer);
			}

			if (refP->direccion == 0) {
				WaitForSingleObject(semPeso, INFINITE);
				refP->cruzando++;
				entro = TRUE;
				LeaveCriticalSection(&csP);
			}
			else {
				LeaveCriticalSection(&csP);
				WaitForSingleObject(cruzarIzq, INFINITE);
			}
		}


		///////////////////////////////////////////////////////////// ANDAR MIENTRAS LA SIGUIENTE ZONA SEA PUENTE ->

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona == PUENTE);


		///////////////////////////////////////////////////////////// DAR UN PASO Y, EN ESTE PUNTO SEGURO QUE SE NECESITA AVISAR AL RESTO DE QUE SE HA ABANDONADO EL PUENTE ->


		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona == PUENTE);

		ReleaseSemaphore(semPeso, 1, NULL);

		EnterCriticalSection(&csP);
		refP->cruzando--;
		if (refP->cruzando == 0) {
			ResetEvent(cruzarIzq);
			SetEvent(cruzarDer);
		}
		LeaveCriticalSection(&csP);



		///////////////////////////////////////////////////////////// ANDAR MIENTRAS LA SIGUIENTE ZONA NO SEA ANTEASALA ->

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona != ANTESALA);

		///////////////////////////////////////////////////////////// QUEDARSE BLOQUEADO SI LA ANTESALA ESTÁ LLENA ->

		WaitForSingleObject(semAnte, INFINITE);

		///////////////////////////////////////////////////////////// ANDAR MIENTRAS LA SIGUIENTE ZONA SEA ANTESALA ->

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona == ANTESALA);

		///////////////////////////////////////////////////////////// QUEDARSE BLOQUEADO MIENTRAS NO HAYA SITIO EN EL COMEDOR ->

		int puestoFinal = -1;
		WaitForSingleObject(semCome, INFINITE);
		WaitForSingleObject(accMemCom, INFINITE);

		LPCH refComBase = (LPCH)MapViewOfFile(memCome, FILE_MAP_ALL_ACCESS, 0, 0, vg->BUF_SIZE_COMEDOR);
		if (refComBase == NULL) {
			PERROR("Error al abrir la memoria del comedor desde un proceso.");
			exit(100);
		}

		LPCH refC = refComBase;
		int elegido;

		srand(GetTickCount64());	// numero aleatorio.
		do {
			elegido = std::rand() % dat_sim->nTenedores;
		} while (refC[elegido * 4] != 1);

		puestoFinal = elegido;
		refC[elegido * 4] = 0;

		UnmapViewOfFile(refComBase);
		int tenedorIzq = (puestoFinal + 8 - 1) % 8;
		int tenedorDer = puestoFinal;
		ReleaseMutex(accMemCom);


		///////////////////////////////////////////////////////////// LLAMAR A LA FUNCION FI_entrarAlComedor, INDICANDO EL SITIO LIBRE ESCOGIDO ->

		vg->FI2_entrarAlComedor(puestoFinal);

		///////////////////////////////////////////////////////////// DAR UN PASO Y, EN ESTE PUNTO SEGURO QUE SE NECESITA AVISAR AL RESTO DE QUE SE HA ABANDONADO LA ANTESALA ->

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona == ANTESALA);

		ReleaseSemaphore(semAnte, 1, NULL);

		///////////////////////////////////////////////////////////// ANDAR MIENTRAS LA SIGUIENTE ZONA SEA ENTRADACOMEDOR ->

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona == ENTRADACOMEDOR);

		///////////////////////////////////////////////////////////// COGER AMBOS TENEDORES, SEGUN EL ALGORITMO USADO, LLAMANDO A LAS FUNCIONES FI_cogerTenedor DOS VECES
		// SOLO SE PUEDE LLAMAR A ESTAS FUNCIONES CUANDO EL TENEDOR AL QUE SE REFIERAN ESTÉ LIBRE ->

		WaitForSingleObject(tene[tenedorIzq], INFINITE);
		WaitForSingleObject(tene[tenedorDer], INFINITE);

		vg->FI2_cogerTenedor(TENEDORDERECHO);
		vg->FI2_cogerTenedor(TENEDORIZQUIERDO);

		///////////////////////////////////////////////////////////// LLAMAR A LA FUNCION FI_comer MIENTRAS DEVUELVA SILLACOMEDOR ->

		while (vg->FI2_comer() == SILLACOMEDOR);

		///////////////////////////////////////////////////////////// DEJAR AMBOS TENEDORES, LLAMANDO A LA FUNCION FI_dejarTenedor POR CADA UNO DE ELLOS ->

		vg->FI2_dejarTenedor(TENEDORDERECHO);
		vg->FI2_dejarTenedor(TENEDORIZQUIERDO);

		ReleaseMutex(tene[tenedorIzq]);
		ReleaseMutex(tene[tenedorDer]);

		///////////////////////////////////////////////////////////// DAR UN PASO Y POSIBLEMENTE INDICAR AL RESTO DE PROCESOS QUE ESE SITIO YA ESTA LIBRE ->

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona == ENTRADACOMEDOR);

		WaitForSingleObject(accMemCom, INFINITE);

		LPCH refComBase2 = (LPCH)MapViewOfFile(memCome, FILE_MAP_ALL_ACCESS, 0, 0, vg->BUF_SIZE_COMEDOR);
		if (refComBase2 == NULL) {
			PERROR("Error al abrir la memoria del comedor desde un proceso.");
			exit(100);
		}

		LPCH refC2 = refComBase2;
		refC2[puestoFinal * 4] = 1;
		for (int i = 0; i < 8; i++) {
			refC2 = refC2 + 4;
		}
		UnmapViewOfFile(refComBase2);

		ReleaseMutex(accMemCom);
		ReleaseSemaphore(semCome, 1, NULL);

		///////////////////////////////////////////////////////////// ANDAR MIENTRAS LA SIGUIENTE ZONA NO SEA PUENTE ->

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona == SALIDACOMEDOR);

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona == CAMPO);


		///////////////////////////////////////////////////////////// REPETIR LAS ACCIONES REALIZADAS ANTERIORMENTE, SOLO QUE AHORA PARA CRUZAR EL PUENTE HACIA LA DERECHA ->

		entro = FALSE;

		while (!entro) {
			EnterCriticalSection(&csP);
			if (refP->cruzando == 0) {
				refP->direccion = 1;
				SetEvent(cruzarDer);
				ResetEvent(cruzarIzq);
			}

			if (refP->direccion == 1) {
				WaitForSingleObject(semPeso, INFINITE);
				refP->cruzando++;
				entro = TRUE;
				LeaveCriticalSection(&csP);
			}
			else {
				LeaveCriticalSection(&csP);
				WaitForSingleObject(cruzarDer, INFINITE);
			}
		}

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;
		} while (nextZona == PUENTE);

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona == PUENTE);


		ReleaseSemaphore(semPeso, 1, NULL);

		EnterCriticalSection(&csP);
		refP->cruzando--;
		if (refP->cruzando == 0) {
			ResetEvent(cruzarDer);
			SetEvent(cruzarIzq);
		}
		LeaveCriticalSection(&csP);


		///////////////////////////////////////////////////////////// UNA VEZ CRUZADO EL PUENTE, ANDAR MIENTRAS LA SIGUIENTE ZONA NO SEA TEMPLO ->

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona != TEMPLO);

		///////////////////////////////////////////////////////////// BLOQUEARSE MIENTRAS NO HAYA AL MENOS UN SITIO LIBRE EN EL TEMPLO ->

		WaitForSingleObject(mutexTemplo, INFINITE);

		LPCH baseTemplo = (LPCH)MapViewOfFile(memTemplo, FILE_MAP_ALL_ACCESS, 0, 0, dat_sim->sitiosTemplo * 4);
		if (baseTemplo == NULL) {
			PERROR("Error al abrir puntero filosofo a memoria compartida del templo");
			exit(100);
		}

		int sitioTemplo;
		do {
			sitioTemplo = rand() % dat_sim->sitiosTemplo;
		} while (baseTemplo[sitioTemplo * 4] != 1); 
		baseTemplo[sitioTemplo * 4] = 0; 

		UnmapViewOfFile(baseTemplo);
		ReleaseMutex(mutexTemplo);

		vg->FI2_entrarAlTemplo(sitioTemplo);


		///////////////////////////////////////////////////////////// ANDAR MIENTRAS LA SIGUIENTE ZONA SEA TEMPLO ->

		do {
			vg->FI2_aDOndeVoySiAndo(&x, &y);
			WaitForSingleObject(coords[x][y], INFINITE);
			vg->FI2_pausaAndar();
			nextZona = vg->FI2_andar();
			ReleaseMutex(coords[x0][y0]);
			x0 = x;
			y0 = y;

		} while (nextZona == TEMPLO);

		///////////////////////////////////////////////////////////// LLAMAR A LA FUNCION FI_meditar MIENTRAS ESTA DEVUELVA SITIOTEMPLO ->

		while (vg->FI2_meditar() == SITIOTEMPLO);

		WaitForSingleObject(mutexTemplo, INFINITE);

		LPCH baseTemplo2 = (LPCH)MapViewOfFile(memTemplo, FILE_MAP_ALL_ACCESS, 0, 0, dat_sim->sitiosTemplo * 4);
		if (baseTemplo2 == NULL) {
			PERROR("Error al abrir puntero filosofo 2 a memoria compartida del templo");
			exit(100);
		}
		baseTemplo2[sitioTemplo * 4] = 1; 

		UnmapViewOfFile(baseTemplo2);
		ReleaseMutex(mutexTemplo);

		vueltas++;

	}
	///////////////////////////////////////////////////////////// FIN DEL BUCLE <-

	///////////////////////////////////////////////////////////// LLAMAR A LA FUNCION FI_finFilOsofo ->

	vg->FI2_finFilOsofo();

	return 0;
}

///////////////////////////////////////////////////////////// THREAD FILOSOFO <-