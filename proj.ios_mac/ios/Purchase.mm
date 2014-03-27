#import "Purchase.h"

static function<void(string)> purchaseCallback;

void Purchase::purchase(const char* productID, function<void(string)> callback) {
    purchaseCallback = callback;
    purchaseCallback("hoge");
}
