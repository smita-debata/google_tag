<?php

namespace Drupal\Tests\google_tag\Functional;

use Drupal\google_tag\Entity\Container;
use Drupal\Tests\BrowserTestBase;

/**
 * Tests the Google Tag Manager.
 *
 * @todo
 * Use the settings form to save configuration and create snippet files.
 * Confirm snippet file and page response contents.
 * Test further the snippet insertion conditions.
 *
 * @group GoogleTag
 */
abstract class GTMTestBase extends BrowserTestBase {

  /**
   * Modules to install.
   *
   * @var array
   */
  public static $modules = ['google_tag'];

  /**
   * The snippet file types.
   *
   * @var array
   */
  protected $types = ['script', 'noscript'];

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();
    $this->basePath = \Drupal::config('google_tag.settings')->get('uri');
  }

  /**
   * Test the module.
   */
  public function testModule() {
    try {
      $this->modifySettings();
      $this->createData();
      $this->checkSnippetFiles();
      $this->checkPageResponse();
    }
    catch (Exception $e) {
      parent::assertTrue(TRUE, t('Inside CATCH block'));
      watchdog_exception('gtm_test', $e);
    }
    finally {
      parent::assertTrue(TRUE, t('Inside FINALLY block'));
    }
  }

  /**
   * Modify settings for test purposes.
   */
  protected function modifySettings() {
    // Modify default settings.
    // These should propagate to each container created in test.
    $config = \Drupal::service('config.factory')->getEditable('google_tag.settings');
    $settings = $config->get();
    unset($settings['_core']);
    $settings['debug_output'] = 1;
    $settings['_default_container']['role_toggle'] = 'include listed';
    $settings['_default_container']['role_list'] = ['content viewer' => 'content viewer'];
    $config->setData($settings)->save();
  }

  /**
   * Create test data: containers and snippet files.
   */
  protected function createData() {
  }

  /**
   * Save container in the database and create snippet files.
   */
  protected function saveContainer(array $variables) {
    // Create container with default container settings, then modify and save.
    $container = new Container([], 'google_tag_container');
    $container->enforceIsNew();
    $container->set('id', $variables['id']);
    unset($variables['id']);
    array_walk($variables, function ($value, $key) use ($container) {
      $container->$key = $value;
    });
    $container->save();

    // Create snippet files.
    $manager = \Drupal::service('google_tag.container_manager');
    $manager->createAssets($container);
  }

  /**
   * Inspect the snippet files.
   */
  protected function checkSnippetFiles() {
  }

  /**
   * Verify the snippet file contents.
   */
  protected function verifyScriptSnippet($contents, $variables) {
    $status = strpos($contents, "'$variables->container_id'") !== FALSE;
    $message = 'Found in script snippet file: container_id';
    parent::assertTrue($status, $message);

    $status = strpos($contents, "gtm_preview=$variables->environment_id") !== FALSE;
    $message = 'Found in script snippet file: environment_id';
    parent::assertTrue($status, $message);

    $status = strpos($contents, "gtm_auth=$variables->environment_token") !== FALSE;
    $message = 'Found in script snippet file: environment_token';
    parent::assertTrue($status, $message);
  }

  /**
   * Verify the snippet file contents.
   */
  protected function verifyNoScriptSnippet($contents, $variables) {
    $status = strpos($contents, "id=$variables->container_id") !== FALSE;
    $message = 'Found in noscript snippet file: container_id';
    parent::assertTrue($status, $message);

    $status = strpos($contents, "gtm_preview=$variables->environment_id") !== FALSE;
    $message = 'Found in noscript snippet file: environment_id';
    parent::assertTrue($status, $message);

    $status = strpos($contents, "gtm_auth=$variables->environment_token") !== FALSE;
    $message = 'Found in noscript snippet file: environment_token';
    parent::assertTrue($status, $message);
  }

  /**
   * Verify the snippet file contents.
   */
  protected function verifyDataLayerSnippet($contents, $variables) {
  }

  /**
   * Inspect the page response.
   */
  protected function checkPageResponse() {
    // Create and login a test user.
    $role_id = $this->drupalCreateRole(['access content'], 'content viewer');
    $non_admin_user = $this->drupalCreateUser();
    $non_admin_user->roles[] = 'content viewer';
    $non_admin_user->save();
    $this->drupalLogin($non_admin_user);
  }

  /**
   * Verify the tag in page response.
   */
  protected function verifyScriptTag($realpath) {
    $query_string = \Drupal::state()->get('system.css_js_query_string') ?: '0';
    $text = "src=\"$realpath?$query_string\"";
    $this->assertSession()->responseContains($text);

    $xpath = "//script[@src=\"$realpath?$query_string\"]";
    $elements = $this->xpath($xpath);
    $status = !empty($elements);
    $message = 'Found script tag in page response';
    parent::assertTrue($status, $message);
  }

  /**
   * Verify the tag in page response.
   */
  protected function verifyNoScriptTag($realpath, $variables) {
    // The tags are sorted by weight.
    $index = isset($variables->weight) ? $variables->weight - 1 : 0;
    $xpath = '//noscript//iframe';
    $elements = $this->xpath($xpath);
    $contents = $elements[$index]->getAttribute('src');

    $status = strpos($contents, "id=$variables->container_id") !== FALSE;
    $message = 'Found in noscript tag: container_id';
    parent::assertTrue($status, $message);

    $status = strpos($contents, "gtm_preview=$variables->environment_id") !== FALSE;
    $message = 'Found in noscript tag: environment_id';
    parent::assertTrue($status, $message);

    $status = strpos($contents, "gtm_auth=$variables->environment_token") !== FALSE;
    $message = 'Found in noscript tag: environment_token';
    parent::assertTrue($status, $message);
  }

}
