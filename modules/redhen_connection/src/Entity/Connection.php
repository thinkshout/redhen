<?php

/**
 * @file
 * Contains \Drupal\redhen_connection\Entity\Connection.
 */

namespace Drupal\redhen_connection\Entity;

use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\redhen_connection\ConnectionInterface;

/**
 * Defines the Connection entity.
 *
 * @ingroup redhen_connection
 *
 * @ContentEntityType(
 *   id = "redhen_connection",
 *   label = @Translation("Connection"),
 *   label_singular = @Translation("Connection"),
 *   label_plural = @Translation("Connections"),
 *   label_count = @PluralTranslation(
 *     singular = "@count Connection",
 *     plural = "@count Connections",
 *   ),
 *   bundle_label = @Translation("Connection type"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\redhen_connection\ConnectionListBuilder",
 *     "views_data" = "Drupal\redhen_connection\Entity\ConnectionViewsData",
 *
 *     "form" = {
 *       "default" = "Drupal\redhen_connection\Form\ConnectionForm",
 *       "add" = "Drupal\redhen_connection\Form\ConnectionForm",
 *       "edit" = "Drupal\redhen_connection\Form\ConnectionForm",
 *       "delete" = "Drupal\redhen_connection\Form\ConnectionDeleteForm",
 *     },
 *     "access" = "Drupal\redhen_connection\ConnectionAccessControlHandler",
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_connection\ConnectionHtmlRouteProvider",
 *     },
 *   },
 *   base_table = "redhen_connection",
 *   revision_table = "redhen_connection_revision",
 *   admin_permission = "administer Connection entities",
 *   entity_keys = {
 *     "id" = "id",
 *     "revision" = "revision_id",
 *     "bundle" = "type",
 *     "uuid" = "uuid",
 *     "uid" = "user_id",
 *     "langcode" = "langcode",
 *     "status" = "status",
 *   },
 *   links = {
 *     "canonical" = "/redhen/connection/{redhen_connection}",
 *     "add-form" = "/redhen/connection/add/{redhen_connection_type}",
 *     "edit-form" = "/redhen/connection/{redhen_connection}/edit",
 *     "delete-form" = "/redhen/connection/{redhen_connection}/delete",
 *     "collection" = "/redhen/connection",
 *   },
 *   bundle_entity_type = "redhen_connection_type",
 *   field_ui_base_route = "entity.redhen_connection_type.edit_form"
 * )
 */
class Connection extends ContentEntityBase implements ConnectionInterface {
  use EntityChangedTrait;

  /**
   * {@inheritdoc}
   */
  public function label() {
    // @TODO make this smarter.
    return $this->get('entity1')->entity->label() . ' : ' . $this->get('entity2')->entity->label();
  }
  
  /**
   * {@inheritdoc}
   */
  public function getType() {
    return $this->bundle();
  }

  /**
   * {@inheritdoc}
   */
  public function getCreatedTime() {
    return $this->get('created')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setCreatedTime($timestamp) {
    $this->set('created', $timestamp);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function isActive() {
    return (bool) $this->getEntityKey('status');
  }

  /**
   * {@inheritdoc}
   */
  public function setActive($active) {
    $this->set('status', $active ? REDHEN_CONNECTION_INACTIVE : REDHEN_CONNECTION_ACTIVE);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = parent::baseFieldDefinitions($entity_type);

    $fields['entity1'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('Entity 1'))
      ->setDescription(t('The first entity this connection connects.'))
      ->setRequired(TRUE)
      ->setDisplayOptions('form', [
        'type' => 'entity_reference_autocomplete',
        'weight' => -1,
        'settings' => [
          'match_operator' => 'CONTAINS',
          'size' => '60',
          'placeholder' => '',
        ],
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['entity2'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('Entity 2'))
      ->setDescription(t('The second entity this connection connects.'))
      ->setRequired(TRUE)
      ->setDisplayOptions('form', [
        'type' => 'entity_reference_autocomplete',
        'weight' => -1,
        'settings' => [
          'match_operator' => 'CONTAINS',
          'size' => '60',
          'placeholder' => '',
        ],
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['status'] = BaseFieldDefinition::create('boolean')
      ->setLabel(t('Active'))
      ->setDescription(t('A boolean indicating whether the connection is active.'))
      ->setDefaultValue(TRUE)
      ->setDisplayOptions('form', array(
        'type' => 'boolean_checkbox',
        'settings' => array(
          'display_label' => TRUE,
        ),
        'weight' => 16,
      ))
      ->setDisplayConfigurable('form', TRUE)
      ->setRevisionable(TRUE);

    $fields['created'] = BaseFieldDefinition::create('created')
      ->setLabel(t('Created'))
      ->setDescription(t('The time that the connection was created.'))
      ->setRevisionable(TRUE);

    $fields['changed'] = BaseFieldDefinition::create('changed')
      ->setLabel(t('Changed'))
      ->setDescription(t('The time that the connection was last edited.'))
      ->setRevisionable(TRUE);

    return $fields;
  }

  /**
   * {@inheritdoc}
   */
  public static function bundleFieldDefinitions(EntityTypeInterface $entity_type, $bundle, array $base_field_definitions) {
    /** @var \Drupal\redhen_connection\ConnectionTypeInterface $connection_type */
    $connection_type = ConnectionType::load($bundle);
    $endpoints = [1, 2];
    $fields = [];
    foreach ($endpoints as $endpoint) {
      $endpoint_type = $connection_type->getEndpointEntityTypeId($endpoint);
      $field = 'entity' . $endpoint;
      $fields[$field] = clone $base_field_definitions[$field];
      if ($endpoint_type) {
        $endpoint_entity = \Drupal::entityManager()->getDefinition($endpoint_type);
        $label = $endpoint_entity->getLabel();
        $fields[$field]->setSetting('target_type', $endpoint_type)
          ->setLabel($label) // @TODO Configurable
          ->setDescription(t('The @type this connection connects.', array('@type' => $label))); // @TODO Configurable
      }
    }
    return $fields;
  }
}
