package main

import (
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/rsa"
	"crypto/x509"
	"encoding/pem"
	"flag"
	"fmt"
	"log"
	"os"

	"golang.org/x/crypto/ssh"
)

var (
	bitsize = flag.Int("b", 0, "bit size")
	keytype = flag.String("t", "rsa", "type of key to create")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("ssh-keygen: ")
	flag.Usage = usage
	flag.Parse()
	if *bitsize == 0 {
		switch *keytype {
		case "rsa":
			*bitsize = 4096
		case "ecdsa":
			*bitsize = 521
		}
	}
	err := gen(*keytype, *bitsize)
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: ssh-keygen [options]")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "supported key types: rsa ecdsa")
	os.Exit(2)
}

func gen(keytype string, bitsize int) error {
	var (
		priv, pub interface{}
		pemblock  pem.Block
	)
	switch keytype {
	case "rsa":
		rsapriv, err := genrsa(bitsize)
		if err != nil {
			return err
		}

		priv = rsapriv
		pub = &rsapriv.PublicKey

		privder, err := x509.MarshalPKCS8PrivateKey(priv)
		if err != nil {
			return err
		}
		pemblock = pem.Block{
			Type:  "RSA PRIVATE KEY",
			Bytes: privder,
		}

	case "ecdsa":
		var curve elliptic.Curve
		switch bitsize {
		case 224:
			curve = elliptic.P224()
		case 256:
			curve = elliptic.P256()
		case 384:
			curve = elliptic.P384()
		case 521:
			curve = elliptic.P521()
		default:
			return fmt.Errorf("unsupported ecdsa size %d", bitsize)
		}
		ecdsapriv, err := ecdsa.GenerateKey(curve, rand.Reader)
		if err != nil {
			return err
		}
		priv = ecdsapriv
		pub = &ecdsapriv.PublicKey

		privec, err := x509.MarshalECPrivateKey(ecdsapriv)
		if err != nil {
			return err
		}

		pemblock = pem.Block{
			Type:  "EC PRIVATE KEY",
			Bytes: privec,
		}

	default:
		return fmt.Errorf("unsupported key type %q", keytype)
	}

	sshpub, err := ssh.NewPublicKey(pub)
	if err != nil {
		return err
	}

	privdata := pem.EncodeToMemory(&pemblock)
	pubdata := ssh.MarshalAuthorizedKey(sshpub)

	fmt.Printf("%s", privdata)
	fmt.Printf("%s", pubdata)

	return err
}

func genrsa(bitsize int) (*rsa.PrivateKey, error) {
	priv, err := rsa.GenerateKey(rand.Reader, bitsize)
	if err != nil {
		return nil, err
	}

	err = priv.Validate()
	if err != nil {
		return nil, err
	}

	return priv, nil
}
