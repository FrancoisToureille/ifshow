#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

// Fonction qui affiche les adresses IP d'une interface réseau spécifique
void show_interface_addresses(const char *ifname) {
    struct ifaddrs *ifaddr, *ifa;
    
    // Récupération des interfaces réseau disponibles
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    // Parcours des interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        if (strcmp(ifa->ifa_name, ifname) != 0) continue;

        int family = ifa->ifa_addr->sa_family;
        char addr[INET6_ADDRSTRLEN];
        
        // Vérification si l'adresse est IPv4 ou IPv6
        if (family == AF_INET || family == AF_INET6) {
            void *addr_ptr = (family == AF_INET) ?
                             (void *)&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr :
                             (void *)&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            inet_ntop(family, addr_ptr, addr, sizeof(addr));
            printf("%s: %s\n", ifa->ifa_name, addr);
        }
    }
    
    // Libération de la mémoire allouée pour les interfaces
    freeifaddrs(ifaddr);
}

// Fonction qui affiche toutes les interfaces réseau et leurs adresses IP
void show_all_interfaces() {
    struct ifaddrs *ifaddr, *ifa;
    
    // Récupération des interfaces réseau disponibles
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    // Parcours des interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        int family = ifa->ifa_addr->sa_family;
        char addr[INET6_ADDRSTRLEN];
        
        // Vérification si l'adresse est IPv4 ou IPv6
        if (family == AF_INET || family == AF_INET6) {
            void *addr_ptr = (family == AF_INET) ?
                             (void *)&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr :
                             (void *)&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            inet_ntop(family, addr_ptr, addr, sizeof(addr));
            printf("%s: %s\n", ifa->ifa_name, addr);
        }
    }
    
    // Libération de la mémoire allouée pour les interfaces
    freeifaddrs(ifaddr);
}

// Fonction principale
int main(int argc, char *argv[]) {
    // Vérifie si l'argument est "-i" suivi d'un nom d'interface
    if (argc == 3 && strcmp(argv[1], "-i") == 0) {
        show_interface_addresses(argv[2]);
    } 
    // Vérifie si l'argument est "-a" pour afficher toutes les interfaces
    else if (argc == 2 && strcmp(argv[1], "-a") == 0) {
        show_all_interfaces();
    } 
    // Affiche l'usage en cas d'arguments invalides
    else {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s -i <interface> : Show IP addresses of specified interface\n", argv[0]);
        fprintf(stderr, "  %s -a : Show all interfaces with their IP addresses\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
