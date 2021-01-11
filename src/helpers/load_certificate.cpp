#include "load_certificate.hpp"
#include <boost/asio/buffer.hpp>
#include <cstddef>
#include <memory>

void
load_certificate(boost::asio::ssl::context& ctx,
		const std::string cert_file_name,
		const std::string key_file_name,
		boost::system::error_code& ec) noexcept
{
	ctx.set_password_callback([](std::size_t,
            boost::asio::ssl::context_base::password_purpose)
        {
            return "test";
        }, ec);
	if(ec) return;

    ctx.set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::single_dh_use,
		ec);
    if(ec) return;

	ctx.use_certificate_chain_file(cert_file_name, ec);
	if(ec) return;

	ctx.use_private_key_file(key_file_name, boost::asio::ssl::context::pem, ec);
	if(ec) return;

//	ctx.use_tmp_dh_file("dh512.pem", ec);
//	if(ec) return;
}

void
load_certificate(boost::asio::ssl::context& ctx,
		const std::string cert_file_name,
		const std::string key_file_name)
{
	boost::system::error_code ec;

	load_certificate(ctx, cert_file_name, key_file_name, ec);
	if(ec) throw ec;
}

void
load_certificate(boost::asio::ssl::context& ctx)
{
    std::string const cert =
        R"(-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIUWyYJDCoEn5nJZklGT6kJ7mBJpPAwDQYJKoZIhvcNAQEL
BQAwgZQxCzAJBgNVBAYTAmJyMRcwFQYDVQQIDA5lc3Bpcml0byBzYW50bzEQMA4G
A1UEBwwHdml0b3JpYTESMBAGA1UECgwJeGVkcyBjb3JwMQswCQYDVQQLDAJ0aTEV
MBMGA1UEAwwMbXlkb21haW4uY29tMSIwIAYJKoZIhvcNAQkBFhNybmFzY3VuaGFA
Z21haWwuY29tMB4XDTIwMDYwMjE5NTMyOVoXDTIyMDkwNTE5NTMyOVowSDELMAkG
A1UEBhMCYnIxFTATBgNVBAMMDG15ZG9tYWluLmNvbTEiMCAGCSqGSIb3DQEJARYT
cm5hc2N1bmhhQGdtYWlsLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC
ggEBALWftVEYTaeSsZfnm2CrigwvHY34ye5d93ErP+odpeT3RnsZSGF+i8v6C5/J
l6uYar2yAdLSFo36lLhDjWTKzg6r4CyJm7IwIBu5L2WsBMzz1E1m9WrG+Iq4gH6v
Yxk4E+9FCe7y7arpiX43OInHwk41iEtP7aM65NP0D93u9kJko3RUxyK0vo7PGS9g
BPKyhAZtdWaC2fS3tGC0mK3htPyAOXpEZ7R2+lRPXQNxzCoCE8XbixTFn5OLDfnF
4EJM3hcJ7GeSr9zvkCqz8698H1J6lt5w3dCO5z2K1VCquspl6WKDDA9jikH0nYQ8
QNkl3Hhy/JnzO0Z/Uw0Tw4HihvkCAwEAAaNaMFgwHwYDVR0jBBgwFoAUvLQl3787
PhVSxuKayuPun9JzDlEwCQYDVR0TBAIwADALBgNVHQ8EBAMCBPAwHQYDVR0RBBYw
FIIMbXlkb21haW4uY29thwR/AAABMA0GCSqGSIb3DQEBCwUAA4IBAQAXNyzlYSPt
E85LLQwSV61SNJYcL4MhZwBipU9w+phs9u5NZ9q3f31JEtIjCnH9ruSskf8H9XHj
lNpiCLqMyaDwdedbrUMeROVnHS0QLRbvPsCegH234lT5fa2H+ccy2Tis6kHZMdky
n3J/6yyQx0P2L9+UMlTuehzQE+t+I4j5uEC4IDXP5xL1sOuY8C3FgHxiQoexVCh7
WtSlp2yjqhijQgwZHay5Sr6HP4up8+c4CFQBJBCdYFw8hNJOO18X4sQNzT6h79OY
tL8bOeK024fkXprTLQ35RYMc66Tw1rFblTbM9o1R5l8mBzezwlc2jtGzzI4iORaK
4SPuLXx3a4fE
-----END CERTIFICATE-----)";

    std::string const key =
        R"(-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAtZ+1URhNp5Kxl+ebYKuKDC8djfjJ7l33cSs/6h2l5PdGexlI
YX6Ly/oLn8mXq5hqvbIB0tIWjfqUuEONZMrODqvgLImbsjAgG7kvZawEzPPUTWb1
asb4iriAfq9jGTgT70UJ7vLtqumJfjc4icfCTjWIS0/tozrk0/QP3e72QmSjdFTH
IrS+js8ZL2AE8rKEBm11ZoLZ9Le0YLSYreG0/IA5ekRntHb6VE9dA3HMKgITxduL
FMWfk4sN+cXgQkzeFwnsZ5Kv3O+QKrPzr3wfUnqW3nDd0I7nPYrVUKq6ymXpYoMM
D2OKQfSdhDxA2SXceHL8mfM7Rn9TDRPDgeKG+QIDAQABAoIBAQCvx3ZwmhjuCeef
oSScFulwiVkNQrAM5xzIhpdBOpiIVVn9YabP1O3v6Hy/lRgZl1YXaeSh6XqyxsaQ
37BE0JNFV8uKz1PEzRcGPeBFgorS4Vmj3/W99fwuJgsWkSBUFBYXCLZocOzc8VGE
umUZ1B+WGfFjEmRoCmPVTuzcWp4z+heqAlpcPys1jr3gOSMZEvKuH06oA+pzh2jW
9t3yBAxq+aYbPPpd1lk6w5pKbPfxue9HjWGYrlTghFx7azeymI3oO9rIMpa5tPyN
N+8LFkUnGa0o2WEW1VU1mPYHd6cOIV7x/KOUyBT+kiPTc200dvQL7r4rRj+SFfxl
/AInAvlBAoGBAOwxD6r2Y5Z32ac/Is/iY/LdRT/WUQaNTx5REddQ9x665xOVJfL2
ppFdAdf5lzl0FkgEORlq9DiZM0V8v6wt30MUP7VXfcUhsLeRFDWnZ7/SZc6mzTTX
ePSWHlg0aoyMgiKQqVYa2xXj3j3aOv3yy/6+2Mte3wNeefrktohJrPUdAoGBAMTb
GU8hUQu4os4DdGxp3f5v6NLX+lgUd8KkW686tva9C5ctB4ItpIiatkJzhWslcDDx
Wrn3HfwG0UPQxBBYXL+0JNpk9n1BfIY5GGdXNq2/BC9J1v22qImwKkxzIeqCX0KN
WhV1hvpvZcccOpInDoTGQ6qk0ogspNsnDgsERL6NAoGBANuf0z3Z1ki4MT9/SdOc
Y+wVLKtrkfRRudwq7jgVJXUBvpVK/vqpt8E7htdZUvEtrPpSUvX7qhRnAHIAygpE
rXImhgcNz/laloRa9zvc6nBIKDq8h9w28FYesSC7vMfsb6XMdmmGl2oHdUR2hJxX
BLFgrR6OnTj4xNwziDe6qoVRAoGBAMLtbcdUZ0ti/htfgmZkAoWTTQmTAfs78UbU
t1DxpAD5O0PsO9Yk2hrGkBmHCKMLhGeCZuNkltVb7tXnIG5LATR27lMvzIBFW0gR
u1+BFczALCWjSXcb6xHEVm4zR29Ov4NIMt6y7QCJVE/wXPMiJ5dEN/b+lNZw1qGH
r7kq9eN1AoGAFMy7aHMWILHfMJAmdwdADyB9Qo8mDK92YqYg7XH+CHuOS7yNKeHI
yGQtriOfabouwe1aXZhV6hYolbRRnDOGIhkhz9b373XwhF1QtQLccwYIMiLafzUE
jqRux+KYoT6MuqXraNf77PBd1+SlpsMz7gGGfgsiUx6GisiPGe5+zDw=
-----END RSA PRIVATE KEY-----)";

//    std::string const dh =
//    		R"(
//)";

    ctx.set_password_callback(
        [](std::size_t,
            boost::asio::ssl::context_base::password_purpose)
        {
            return "test";
        });

    ctx.set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::single_dh_use);

    ctx.use_certificate_chain(
        boost::asio::buffer(cert.data(), cert.size()));

    ctx.use_private_key(
        boost::asio::buffer(key.data(), key.size()),
        boost::asio::ssl::context::file_format::pem);

//    ctx.use_tmp_dh(
//        boost::asio::buffer(dh.data(), dh.size()));
}
