#include <core.p4>
#include <ubpf_model.p4>

header header_one {
    bit<8> type;
    bit<8> data;
}

header header_two {
    bit<8>  type;
    bit<16> data;
}

header header_four {
    bit<8>  type;
    bit<32> data;
}

struct Headers_t {
    header_one  one;
    header_two  two;
    header_four four;
}

struct metadata {
}

parser prs(packet_in p, out Headers_t headers, inout metadata meta, inout standard_metadata std_meta) {
    bit<8> tmp_1;
    state start {
        transition parse_headers;
    }
    state parse_headers {
        tmp_1 = p.lookahead<bit<8>>();
        transition select(tmp_1) {
            8w1: parse_one;
            8w2: parse_two;
            8w4: parse_four;
            default: accept;
        }
    }
    state parse_one {
        p.extract<header_one>(headers.one);
        transition parse_headers;
    }
    state parse_two {
        p.extract<header_two>(headers.two);
        transition parse_headers;
    }
    state parse_four {
        p.extract<header_four>(headers.four);
        transition parse_headers;
    }
}

control pipe(inout Headers_t headers, inout metadata meta, inout standard_metadata std_meta) {
    apply {
    }
}

control dprs(packet_out packet, in Headers_t headers) {
    @hidden action lookahead_ubpf61() {
        packet.emit<header_one>(headers.one);
        packet.emit<header_two>(headers.two);
        packet.emit<header_four>(headers.four);
    }
    @hidden table tbl_lookahead_ubpf61 {
        actions = {
            lookahead_ubpf61();
        }
        const default_action = lookahead_ubpf61();
    }
    apply {
        tbl_lookahead_ubpf61.apply();
    }
}

ubpf<Headers_t, metadata>(prs(), pipe(), dprs()) main;
