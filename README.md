# SO
## Instrucciones de compilacion
1. Compilar chasquiEats.c con el comando `gcc -o chasquiEats chasquiEats.c -lrt`
2. Compilar client.c con el comando `gcc -o client client.c -lrt -lpthread`
3. Compilar restaurant.c con el comando `gcc -o restaurant restaurant.c -lrt -lpthread`
4. Compilar motorizado.c con el comando `gcc -o motorizado motorizado.c -lrt -lpthread`
## Instrucciones de ejecucion
1. Ejecutar chasquiEats con el comando `./chasquiEats <dimension_matriz> <num_restaurantes> <intervalo_ms> <num_motorizados> <distancia_km>`
3. Ejecutar restaurant con el comando `./restaurant`
2. Ejecutar client con el comando `./client`
4. Ejecutar motorizado con el comando `./motorizado`
