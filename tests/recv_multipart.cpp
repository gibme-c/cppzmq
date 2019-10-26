#include <catch.hpp>
#include <zmq_addon.hpp>

#ifdef ZMQ_CPP11

TEST_CASE("recv_multipart test", "[recv_multipart]")
{
    zmq::context_t context(1);
    zmq::socket_t output(context, ZMQ_PAIR);
    zmq::socket_t input(context, ZMQ_PAIR);
    output.bind("inproc://multipart.test");
    input.connect("inproc://multipart.test");

    SECTION("send 1 message")
    {
        input.send(zmq::str_buffer("hello"));

        std::vector<zmq::message_t> msgs;
        auto ret = zmq::recv_multipart(output, std::back_inserter(msgs));
        REQUIRE(ret);
        CHECK(*ret == 1);
        REQUIRE(msgs.size() == 1);
        CHECK(msgs[0].size() == 5);
    }
    SECTION("send 2 messages")
    {
        input.send(zmq::str_buffer("hello"), zmq::send_flags::sndmore);
        input.send(zmq::str_buffer("world!"));

        std::vector<zmq::message_t> msgs;
        auto ret = zmq::recv_multipart(output, std::back_inserter(msgs));
        REQUIRE(ret);
        CHECK(*ret == 2);
        REQUIRE(msgs.size() == 2);
        CHECK(msgs[0].size() == 5);
        CHECK(msgs[1].size() == 6);
    }
    SECTION("send no messages, dontwait")
    {
        std::vector<zmq::message_t> msgs;
        auto ret = zmq::recv_multipart(output, std::back_inserter(msgs), zmq::recv_flags::dontwait);
        CHECK_FALSE(ret);
        REQUIRE(msgs.size() == 0);
    }
    SECTION("send 1 partial message, dontwait")
    {
        input.send(zmq::str_buffer("hello"), zmq::send_flags::sndmore);

        std::vector<zmq::message_t> msgs;
        auto ret = zmq::recv_multipart(output, std::back_inserter(msgs), zmq::recv_flags::dontwait);
        CHECK_FALSE(ret);
        REQUIRE(msgs.size() == 0);
    }
    SECTION("recv with invalid socket")
    {
        std::vector<zmq::message_t> msgs;
        CHECK_THROWS_AS(zmq::recv_multipart(zmq::socket_ref(), std::back_inserter(msgs)), const zmq::error_t &);
    }
}
#endif
