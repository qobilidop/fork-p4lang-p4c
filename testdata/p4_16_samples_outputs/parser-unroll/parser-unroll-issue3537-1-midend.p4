#include <core.p4>
#define V1MODEL_VERSION 20180101
#include <v1model.p4>

struct H {
}

struct M {
}

parser ParserI(packet_in packet, out H hdr, inout M meta, inout standard_metadata_t smeta) {
    bit<16> tmp_3;
    bit<16> tmp_4;
    state s1 {
        packet.advance(32w16);
        tmp_3 = packet.lookahead<bit<16>>();
        transition select(tmp_3) {
            16w0: s1;
            default: s2;
        }
    }
    state s2 {
        packet.advance(32w16);
        tmp_4 = packet.lookahead<bit<16>>();
        transition select(tmp_4) {
            16w0: s1;
            16w1: s2;
            default: s4;
        }
    }
    state s4 {
        transition accept;
    }
    state start {
        transition s1;
    }
}

control IngressI(inout H hdr, inout M meta, inout standard_metadata_t smeta) {
    apply {
    }
}

control EgressI(inout H hdr, inout M meta, inout standard_metadata_t smeta) {
    apply {
    }
}

control DeparserI(packet_out pk, in H hdr) {
    apply {
    }
}

control VerifyChecksumI(inout H hdr, inout M meta) {
    apply {
    }
}

control ComputeChecksumI(inout H hdr, inout M meta) {
    apply {
    }
}

V1Switch<H, M>(ParserI(), VerifyChecksumI(), IngressI(), EgressI(), ComputeChecksumI(), DeparserI()) main;
