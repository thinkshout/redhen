<?php

/**
 * @file
 * Contains \Drupal\redhen_relation\Entity\Relation.
 */

namespace Drupal\redhen_relation\Entity;

use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\redhen_relation\RelationInterface;

/**
 * Defines the Relation entity.
 *
 * @ingroup redhen_relation
 *
 * @ContentEntityType(
 *   id = "redhen_relation",
 *   label = @Translation("Relation"),
 *   label_singular = @Translation("Relation"),
 *   label_plural = @Translation("Relations"),
 *   label_count = @PluralTranslation(
 *     singular = "@count Relation",
 *     plural = "@count Relations",
 *   ),
 *   bundle_label = @Translation("Relation type"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\redhen_relation\RelationListBuilder",
 *     "views_data" = "Drupal\redhen_relation\Entity\RelationViewsData",
 *
 *     "form" = {
 *       "default" = "Drupal\redhen_relation\Form\RelationForm",
 *       "add" = "Drupal\redhen_relation\Form\RelationForm",
 *       "edit" = "Drupal\redhen_relation\Form\RelationForm",
 *       "delete" = "Drupal\redhen_relation\Form\RelationDeleteForm",
 *     },
 *     "access" = "Drupal\redhen_relation\RelationAccessControlHandler",
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_relation\RelationHtmlRouteProvider",
 *     },
 *   },
 *   base_table = "redhen_relation",
 *   revision_table = "redhen_relation_revision",
 *   admin_permission = "administer Relation entities",
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
 *     "canonical" = "/redhen/relation/{redhen_relation}",
 *     "add-form" = "/redhen/relation/add/{redhen_relation_type}",
 *     "edit-form" = "/redhen/relation/{redhen_relation}/edit",
 *     "delete-form" = "/redhen/relation/{redhen_relation}/delete",
 *     "collection" = "/redhen/relation",
 *   },
 *   bundle_entity_type = "redhen_relation_type",
 *   field_ui_base_route = "entity.redhen_relation_type.edit_form"
 * )
 */
class Relation extends ContentEntityBase implements RelationInterface {
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
    $this->set('status', $active ? REDHEN_RELATION_INACTIVE : REDHEN_RELATION_ACTIVE);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = parent::baseFieldDefinitions($entity_type);

    $fields['entity1'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('Entity 1'))
      ->setDescription(t('The first entity this relation connects.'))
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
      ->setDescription(t('The second entity this relation connects.'))
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
      ->setDescription(t('A boolean indicating whether the relation is active.'))
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
      ->setDescription(t('The time that the relation was created.'))
      ->setRevisionable(TRUE);

    $fields['changed'] = BaseFieldDefinition::create('changed')
      ->setLabel(t('Changed'))
      ->setDescription(t('The time that the relation was last edited.'))
      ->setRevisionable(TRUE);

    return $fields;
  }

  /**
   * {@inheritdoc}
   */
  public static function bundleFieldDefinitions(EntityTypeInterface $entity_type, $bundle, array $base_field_definitions) {
    /** @var \Drupal\redhen_relation\RelationTypeInterface $relation_type */
    $relation_type = RelationType::load($bundle);
    $endpoints = [1, 2];
    $fields = [];
    foreach ($endpoints as $endpoint) {
      $endpoint_type = $relation_type->getEndpointEntityTypeId($endpoint);
      $field = 'entity' . $endpoint;
      $fields[$field] = clone $base_field_definitions[$field];
      if ($endpoint_type) {
        $endpoint_entity = \Drupal::entityManager()->getDefinition($endpoint_type);
        $label = $endpoint_entity->getLabel();
        $fields[$field]->setSetting('target_type', $endpoint_type)
          ->setLabel($label)
          ->setDescription(t('The @type this relation connects.', array('@type' => $label)));
      }
    }
    return $fields;
  }
}
