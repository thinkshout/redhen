<?php

namespace Drupal\Tests\redhen_connection\Kernel;

use Drupal\KernelTests\KernelTestBase;
use \Drupal\redhen_contact\Entity\Contact;
use \Drupal\redhen_connection\Entity\Connection;
use \Drupal\redhen_connection\Entity\ConnectionType;
use \Drupal\redhen_connection\Entity\ConnectionRole;
use \Drupal\redhen_org\Entity\Org;

/**
 * Tests the ConnectionEntity class.
 *
 * @group redhen
 */
class ConnectionEntityTest extends KernelTestBase {

  /**
   * Enable modules.
   *
   * @var array
   */
  public static $modules = [
    'entity',
    'redhen_contact',
    'redhen_connection',
    'redhen_org',
    'user'
  ];

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();

    $this->installEntitySchema('user');
    $this->installEntitySchema('redhen_connection');
    $this->installEntitySchema('redhen_org');
    $this->installConfig('redhen_org');
  }


  /**
   * Tests the ability for a contact to access an organization.
   */
  public function testHasRolePermission() {
    /* @var \Drupal\redhen_org\Entity\Org $org */
    $org = new Org(array(
        'type' => 'planet',
        'status' => 1,
      ),
      'redhen_org'
    );
    $org->id = 1;

    /* @var \Drupal\redhen_contact\Entity\Contact $contact */
    $contact = new Contact(array(
        'type' => 'friend',
        'status' => 1,
      ),
      'redhen_contact'
    );
    $contact->id = 1;

    /* @var \Drupal\redhen_connection\Entity\ConnectionType $connection_type */
    $connection_type = new ConnectionType(array(
        'id' => 'protector',
        'status' => 1,
        'endpoints' => array(
          array(
            'entity_type' => 'redhen_contact',
            'bundles' => array('friend' => 'friend')
          ),
          array(
            'entity_type' => 'redhen_org',
            'bundles' => array('planet' => 'planet')
          ),
        )
      ),
      'redhen_connection_type'
    );
    $connection_type->save();

    /* @var \Drupal\redhen_connection\Entity\ConnectionRole $connection_role */
    $connection_role = new ConnectionRole(array(
        'id' => 'protector_role',
        'status' => 1,
        'connection_type' => 'protector',
        'permissions' => array(
          'connection' => array(),
          'entity' => array('view'),
          'contact' => array()
        )
      ),
      'redhen_connection_role'
    );
    $connection_role->save();

    /* @var \Drupal\redhen_connection\Entity\Connection $connection_entity */
    $connection_entity = new Connection(array(
      ),
      'redhen_connection',
      'protector'
    );
    $connection_entity->uuid = '723044e1-7f3f-40e0-ab5d-2d433fb1bd01';
    $connection_entity->langcode = 'en';
    $connection_entity->status = 1;
    $connection_entity->role = 'protector_role';
    $connection_entity->type = 'protector';
    $connection_entity->endpoint_1 = 1;
    $connection_entity->endpoint_2 = 1;
    $connection_entity->save();

    $has_permission = $connection_entity->hasRolePermission($org, 'view', $contact);

    $this->assertTrue($has_permission);
  }
}
