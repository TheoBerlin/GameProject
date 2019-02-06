#include <Catch/catch.hpp>
#include <Engine/Events/EventBus.h>

class EventSubscriber
{
public:
    EventSubscriber() {
        EventBus::get().subscribe(this, &EventSubscriber::handleEvent);
    };

    void unsubscribe() {
        EventBus::get().unsubscribe(this, &EventSubscriber::handleEvent);
    };

    int getNum() { return myNum; };

private:
    int myNum = 0;

    void handleEvent(KeyEvent* event) {
        myNum += event->key;
    };
};

TEST_CASE("EventBus") {
    EventSubscriber testSubscriber1;

    SECTION("Sends published events to subscribed instances") {
        // Event to publish
        KeyEvent testEvent(1, 10);

        int expectedNum1 = testSubscriber1.getNum() + testEvent.key;

        EventBus::get().publish(&testEvent);

        REQUIRE(testSubscriber1.getNum() == expectedNum1);

        SECTION("Sends published events to multiple subscribed instances") {
            EventSubscriber testSubscriber2;

            expectedNum1 = testSubscriber1.getNum() + testEvent.key;
            int expectedNum2 = testSubscriber2.getNum() + testEvent.key;

            EventBus::get().publish(&testEvent);

            REQUIRE(testSubscriber1.getNum() == expectedNum1);
            REQUIRE(testSubscriber2.getNum() == expectedNum2);

            SECTION("Allows subscribers to unsubscribe") {
                testSubscriber1.unsubscribe();

                expectedNum1 = testSubscriber1.getNum();
                // Test that unsubscribing one instance does not unsubscribe another
                expectedNum2 = testSubscriber2.getNum() + testEvent.key;

                EventBus::get().publish(&testEvent);

                REQUIRE(expectedNum1 == testSubscriber1.getNum());
                REQUIRE(expectedNum2 == testSubscriber2.getNum());

                SECTION("Does not cause exceptions when non-subscriber unsubscribes") {
                    REQUIRE_NOTHROW(testSubscriber1.unsubscribe());
                }
            }
        }
    }
}
