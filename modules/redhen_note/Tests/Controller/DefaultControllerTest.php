<?php

namespace Drupal\redhen_note\Tests;

use Drupal\simpletest\WebTestBase;

/**
 * Provides automated tests for the redhen_note module.
 */
class DefaultControllerTest extends WebTestBase {
  /**
   * {@inheritdoc}
   */
  public static function getInfo() {
    return array(
      'name' => "redhen_note DefaultController's controller functionality",
      'description' => 'Test Unit for module redhen_note and controller DefaultController.',
      'group' => 'Other',
    );
  }

  /**
   * {@inheritdoc}
   */
  public function setUp() {
    parent::setUp();
  }

  /**
   * Tests redhen_note functionality.
   */
  public function testDefaultController() {
    // Check that the basic functions of module redhen_note.
    $this->assertEquals(TRUE, TRUE, 'Test Unit Generated via App Console.');
  }

}
