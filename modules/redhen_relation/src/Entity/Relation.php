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
    return $this->getName();
  }

  /**
   * {@inheritdoc}
   */
  public function getName() {
    $name = $this->get('name')->value;
    // Allow other modules to alter the name of the Relation.
    \Drupal::moduleHandler()->alter('redhen_relation_name', $name, $this);
    return $name;
  }

  /**
   * {@inheritdoc}
   */
  public function setName($name) {
    $this->set('name', $name);
    return $this;
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
    $this->set('status', $active ? redhen_relation_INACTIVE : redhen_relation_ACTIVE);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = parent::baseFieldDefinitions($entity_type);

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

}
