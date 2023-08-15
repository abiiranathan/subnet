#include "subnet.h"
#include <stdio.h>
#include <stdlib.h>

// gcc -Wall -Werror -pedantic -ggdb -fsanitize=address main.c subnet.c &&
// ./a.out

int main() {
  // Create a subnet with IP address 192.168.1.0 and prefix length 24
  Subnet subnet = create_subnet(0xC0A80100U, 24);

  char *subnetIPString = to_ipv4(subnet.ip).addr;
  printf("Subnet IP: %s\n", subnetIPString);

  // Classify an IP address
  IPAddress ipToClassify = {0xC0A80101U, 0}; // IP: 192.168.1.1
  char ipClass = classify_ip(ipToClassify);

  char *ipToClassifyString = to_ipv4(ipToClassify.ip).addr;
  printf("IP Address: %s is in class %c\n", ipToClassifyString, ipClass);

  // Compute the assignable IP range
  IPAddress rangeStart, rangeEnd;
  assignable_range(subnet, &rangeStart, &rangeEnd);

  char *rangeStartString = to_ipv4(rangeStart.ip).addr;
  char *rangeEndString = to_ipv4(rangeEnd.ip).addr;

  printf("Assignable IP Range: %s - %s\n", rangeStartString, rangeEndString);
  printf("Total Assignable: %u\n", compute_assignable_addresses(subnet));

  // Create subnet from string
  const char *subnet_string = "192.168.4.192/26";
  subnet = create_subnet_from_string(subnet_string);

  char *addr = to_ipv4(subnet.ip).addr;
  char *mask = to_ipv4(subnet.mask).addr;

  printf("IP address: %s\n", addr);
  printf("Mask address: %s\n", mask);
  print_ips_in_subnet(subnet);

  size_t num_ips;
  IPV4 *ip_addresses = get_ips_in_subnet(subnet, &num_ips);

  printf("IP addresses in the subnet are: %zu\n", num_ips);
  for (size_t i = 0; i < num_ips; i++) {
    printf("%s\n", ip_addresses[i].addr);
  }
  free(ip_addresses);

  // Generate subnet table
  printf("\n\nNetwork ID: 192.168.4.0/24\n");
  printf("Table for creation of 4 subnets:\n");

  SubnetInfo *subnet_info = get_subnet_table("192.168.4.0/23", 10);
  printf("\n\nNew Subnet Mask: %s\n", subnet_info->subnet_mask);
  print_subnet_table(subnet_info, 10);
  free(subnet_info);

  return 0;
}
