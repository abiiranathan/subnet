#include "subnet.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

// Function to print help message
void print_help() {
  printf("Usage: subnet-cli [OPTIONS]\n");
  printf("Options:\n");
  printf("  -c, --classify  <ip>            Classify an IP address\n");
  printf("  -r, --range     <ip>            Compute assignable IP range\n");
  printf("  -s, --string    <string>        Create subnet from string\n");
  printf("  -t, --table <network> <subnets> Generate subnet information "
         "table\n");
  printf("  -h, --help                      Print this help message\n");
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    print_help();
    exit(1);
  }

  int option;
  char *ip = NULL;
  char *string = NULL;
  char *network = NULL;
  char *subnets = NULL;

  // Parse command line options using getopt_long
  struct option long_options[] = {{"classify", required_argument, 0, 'c'},
                                  {"range", required_argument, 0, 'r'},
                                  {"string", required_argument, 0, 's'},
                                  {"table", required_argument, 0, 't'},
                                  {"help", no_argument, 0, 'h'},
                                  {0, 0, 0, 0}};

  while ((option = getopt_long(argc, argv, "c:r:s:t:h", long_options, NULL)) !=
         -1) {
    switch (option) {
    case 'c':
      ip = optarg;
      Subnet subnet = create_subnet_from_string(ip);
      char ipClass = classify_ip((IPAddress){.ip = subnet.ip});
      printf("IP Address: %s is in class %c\n", optarg, ipClass);
      break;
    case 'r':
      ip = optarg;
      Subnet sn = create_subnet_from_string(ip);
      IPAddress rangeStart, rangeEnd;
      assignable_range(sn, &rangeStart, &rangeEnd);

      char *rangeStartString = to_ipv4(rangeStart.ip).addr;
      char *rangeEndString = to_ipv4(rangeEnd.ip).addr;

      printf("Assignable IP Range: %s - %s\n", rangeStartString,
             rangeEndString);
      printf("Total Assignable: %u\n", compute_assignable_addresses(sn));
      break;
    case 's':
      string = optarg;
      Subnet subnetFromString = create_subnet_from_string(string);
      char *addr = to_ipv4(subnetFromString.ip).addr;
      char *mask = to_ipv4(subnetFromString.mask).addr;

      printf("IP address: %s\n", addr);
      printf("Mask address: %s\n", mask);
      print_ips_in_subnet(subnetFromString);
      break;
    case 't':
      network = optarg;
      subnets = argv[optind]; // missing subnets argument
      if (subnets == NULL) {
        print_help();
        exit(EXIT_FAILURE);
      }

      SubnetInfo *subnet_info = get_subnet_table(network, atoi(subnets));
      printf("Subnet Mask: %s\n", subnet_info->subnet_mask);
      print_subnet_table(subnet_info, atoi(subnets));
      free(subnet_info);
      break;
    case 'h':
      print_help();
      return 0;
    case '?':
      // Invalid option
      print_help();
      return 1;
    default:
      print_help();
      return 1;
    }
  }

  return 0;
}
