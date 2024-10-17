; Example resource record
; To lookup records on the localhost running the bind server:
; dig @127.0.0.1 example.com (SOA record)
; dig @127.0.0.1 joe.example.com (joe A record)
; dig @127.0.0.1 www.example.com (www A record)
; dig @127.0.0.1 example.com MX (MX records)

; base zone file for example.com
$TTL 2d    ; default TTL for zone

; The symbol @ is used in BIND to denote zone root (which is sometimes also called the zone apex. The value substituted for @ is either:
; The last $ORIGIN directive encountered in the file.
; If no $ORIGIN directive is present - BIND synthesizes one from the value of the zone name in the named.conf file

; @ORIGIN indicates the domain name to be added to all records following this line. This allows the administrator to limit the entry of records.
; Example: :
; @ORIGIN mondc.
; raspi1                  A       172.21.0.1
; The full domain name of this record will be “raspi1.mondc.”.

$ORIGIN example.com. ; base domain-name

; Start of Authority RR defining the key characteristics of the zone (domain)
@         IN      SOA   ns1.example.com. hostmaster.example.com. (
                                2003080800 ; serial number
                                12h        ; refresh
                                15m        ; update retry
                                3w         ; expiry
                                2h         ; minimum)

; NS describes the DNS server of the zone, you can have several records of this type
; name server RR for the domain
           IN      NS      ns1.example.com.
; the second name server is external to this zone (domain)
           IN      NS      ns2.example.net.

; MX describes the email server of the zone, you can have several records of this type. MX is always followed by a number that represents its priority over other MX records in the zone 

; mail server RRs for the zone (domain)
        3w IN      MX  10  mail.example.com.
; the second  mail servers is  external to the zone (domain)
           IN      MX  20  mail.example.net.

; domain hosts includes NS and MX records defined above
; plus any others required
; for instance a user query for the A RR of joe.example.com will
; return the IPv4 address 192.168.254.6 from this zone file
ns1        IN      A       192.168.254.2
mail       IN      A       192.168.254.4
joe        IN      A       192.168.254.6
www        IN      A       192.168.254.7

; aliases ftp (ftp server) to an external domain
ftp        IN      CNAME   ftp.example.net.
