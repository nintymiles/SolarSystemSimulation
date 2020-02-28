//
//  ViewController.m
//  GLPIFramework
//
//  Created by macbook on 9/8/13.
//  Copyright (c) 2013 macbook. All rights reserved.
//

#import "ViewController.h"
#include "NativeTemplate.h"
#include "SolarSystemModel.h"


@interface ViewController () {
}
@property (strong, nonatomic) EAGLContext *context;

@property (retain, nonatomic) IBOutlet UILabel *timescaleLabel;
- (void)initializeGL;
- (void)tearDownGL;

@end


@implementation ViewController

- (void)dealloc
{
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [_context release];
    [_timescaleLabel release];
    [super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3] autorelease];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    NSString *timeScale = [NSString stringWithFormat:@"TimeScale:%.4f",SolarSystemModel::retrieveTimeScale()];
    [_timescaleLabel setText:timeScale];
    
    [self setupGL];
}

- (BOOL) shouldAutorotate
{
    [EAGLContext setCurrentContext:self.context];
    float scale = [UIScreen mainScreen].nativeScale;
    GraphicsResize(self.view.bounds.size.width*scale, self.view.bounds.size.height*scale);
    return true;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *touch;
	CGPoint pos;
	
    float scale = [UIScreen mainScreen].nativeScale;
	for( touch in touches )
	{
		pos = [ touch locationInView:self.view ];
		TouchEventDown( pos.x*scale, pos.y*scale );
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *touch;
	CGPoint pos;
	
    float scale = [UIScreen mainScreen].nativeScale;
	for( touch in touches )
	{
		pos = [ touch locationInView:self.view ];
		TouchEventMove( pos.x*scale, pos.y*scale );
		
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *touch;
	CGPoint pos;
	
    float scale = [UIScreen mainScreen].nativeScale;
	for( touch in touches )
	{
		pos = [ touch locationInView:self.view ];
		
		TouchEventRelease( pos.x*scale, pos.y*scale );
	}
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    
    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }
    
    // Dispose of any resources that can be recreated.
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    [((GLKView *) self.view) bindDrawable];
    //Optional code to demonstrate how can you bind frame buffer and render buffer.
    GLint defaultFBO;
    GLint defaultRBO;
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &defaultRBO);
    
    glBindFramebuffer( GL_FRAMEBUFFER, defaultFBO );
    glBindRenderbuffer( GL_RENDERBUFFER, defaultRBO );

    GraphicsInit();
    
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    GraphicsRender();
}


- (IBAction)increaseTimescale:(id)sender {
    AdjustTimeScale(true,2);
    NSString *timeScale = [NSString stringWithFormat:@"TimeScale:%.4f",SolarSystemModel::retrieveTimeScale()];
    [_timescaleLabel setText:timeScale];
}

- (IBAction)decreaseTimescalse:(id)sender {
    AdjustTimeScale(false,2);
    NSString *timeScale = [NSString stringWithFormat:@"TimeScale:%.4f",SolarSystemModel::retrieveTimeScale()];
    [_timescaleLabel setText:timeScale];
}

- (IBAction)adjustFOV:(id)sender {
    AdjustFOV(0);
}
- (IBAction)toRightSide:(id)sender {
    AdjustFOV(6);
}
- (IBAction)adjustFOVAdversly:(id)sender {
    AdjustFOV(1);
}
- (IBAction)toLeftSide:(id)sender {
    AdjustFOV(5);
}
- (IBAction)switchPlanetView:(id)sender {
    AdjustFOV(4);
}
- (IBAction)adjustInclination:(id)sender {
    AdjustFOV(2);
}
- (IBAction)adjustInclinationAdversly:(id)sender {
    AdjustFOV(3);
}


@end
