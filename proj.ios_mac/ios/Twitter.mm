#import "Twitter.h"
#import <Social/Social.h>

void Twitter::tweet(const char *message, const char *ssFilePath) {
    SLComposeViewController *cvc = [SLComposeViewController composeViewControllerForServiceType:SLServiceTypeTwitter];
    [cvc setInitialText:[NSString stringWithFormat:@"%s", message]];
    [cvc addImage:[UIImage imageWithContentsOfFile:[NSString stringWithFormat:@"%s", ssFilePath]]];
    [[UIApplication sharedApplication].keyWindow.rootViewController presentViewController:cvc animated:YES completion:nil];
}
