<?php

/**
 * @file
 * Contains \Drupal\redhen_contact\Entity\Contact.
 */

namespace Drupal\redhen_contact\Entity;

use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\redhen_contact\ContactInterface;
use Drupal\entity\EntityKeysFieldsTrait;
use Drupal\user\UserInterface;

/**
 * Defines the Contact entity.
 *
 * @ingroup redhen_contact
 *
 * @ContentEntityType(
 *   id = "redhen_contact",
 *   label = @Translation("Contact"),
 *   bundle_label = @Translation("Contact type"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\redhen_contact\ContactListBuilder",
 *     "views_data" = "Drupal\redhen_contact\Entity\ContactViewsData",
 *
 *     "form" = {
 *       "default" = "Drupal\redhen_contact\Form\ContactForm",
 *       "add" = "Drupal\redhen_contact\Form\ContactForm",
 *       "edit" = "Drupal\redhen_contact\Form\ContactForm",
 *       "delete" = "Drupal\redhen_contact\Form\ContactDeleteForm",
 *     },
 *     "access" = "Drupal\redhen_contact\ContactAccessControlHandler",
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_contact\ContactHtmlRouteProvider",
 *     },
 *   },
 *   base_table = "redhen_contact",
 *   revision_table = "redhen_contact_revision",
 *   admin_permission = "administer contact entities",
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
 *     "canonical" = "/redhen/contact/{redhen_contact}",
 *     "add-form" = "/redhen/contact/add/{redhen_contact_type}",
 *     "edit-form" = "/redhen/contact/{redhen_contact}/edit",
 *     "delete-form" = "/redhen/contact/{redhen_contact}/delete",
 *     "collection" = "/redhen/contact",
 *   },
 *   bundle_entity_type = "redhen_contact_type",
 *   field_ui_base_route = "entity.redhen_contact_type.edit_form"
 * )
 */
class Contact extends ContentEntityBase implements ContactInterface {
  use EntityChangedTrait, EntityKeysFieldsTrait;
  /**
   * {@inheritdoc}
   */
  public static function preCreate(EntityStorageInterface $storage_controller, array &$values) {
    parent::preCreate($storage_controller, $values);
    $values += array(
      'user_id' => \Drupal::currentUser()->id(),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function label() {
    return $this->getFullName();
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
  public function getFullName() {
    $first_name = $this->get('first_name')->value;
    $middle_name = $this->get('middle_name')->value;
    $last_name = $this->get('last_name')->value;
    $name = $first_name . (empty($middle_name) ? '' : ' ') . $middle_name . (empty($first_name) ? '' : ' ') . $last_name;
    // Allow other modules to alter the full name of the contact.
    \Drupal::moduleHandler()->alter('redhen_contact_name', $name, $this);
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
  public function getEmail() {
    return $this->get('email')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setEmail($email) {
    $this->set('email', $email);
    return $this;
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
  public function getRevisionCreationTime() {
    return $this->get('revision_timestamp')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setRevisionCreationTime($timestamp) {
    $this->set('revision_timestamp', $timestamp);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getRevisionAuthor() {
    return $this->get('revision_uid')->entity;
  }

  /**
   * {@inheritdoc}
   */
  public function setRevisionAuthorId($uid) {
    $this->set('revision_uid', $uid);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getUser() {
    return $this->get('user_id')->entity;
  }

  /**
   * {@inheritdoc}
   */
  public function getUserId() {
    return $this->get('user_id')->target_id;
  }

  /**
   * {@inheritdoc}
   */
  public function setUserId($uid) {
    $this->set('user_id', $uid);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function setUser(UserInterface $account) {
    $this->set('user_id', $account->id());
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
    $this->set('status', $active ? REDHEN_CONTACT_INACTIVE : REDHEN_CONTACT_ACTIVE);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = self::entityKeysBaseFieldDefinitions($entity_type);

    $fields['first_name'] = BaseFieldDefinition::create('string')
      ->setLabel(t('First name'))
      ->setDescription(t('The first name of the contact.'))
      ->setSettings(array(
        'max_length' => 50,
        'text_processing' => 0,
      ))
      ->setDefaultValue('')
      ->setDisplayOptions('form', array(
        'type' => 'string_textfield',
        'weight' => -10,
      ))
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE)
      ->setRevisionable(TRUE);

    $fields['middle_name'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Middle Name'))
      ->setDescription(t('The middle name of the contact.'))
      ->setSettings(array(
        'max_length' => 50,
        'text_processing' => 0,
      ))
      ->setDefaultValue('')
      ->setDisplayOptions('form', array(
        'type' => 'string_textfield',
        'weight' => -9,
      ))
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE)
      ->setRevisionable(TRUE);

    $fields['last_name'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Last Name'))
      ->setDescription(t('The last name of the contact.'))
      ->setSettings(array(
        'max_length' => 50,
        'text_processing' => 0,
      ))
      ->setDefaultValue('')
      ->setDisplayOptions('form', array(
        'type' => 'string_textfield',
        'weight' => -8,
      ))
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE)
      ->setRevisionable(TRUE);


    $fields['email'] = BaseFieldDefinition::create('email')
      ->setLabel(t('Email'))
      ->setDescription(t('The email of this contact.'))
      ->setDefaultValue('')
      ->setDisplayOptions('view', array(
        'label' => 'inline',
        'type' => 'email_mailto',
        'weight' => -4,
      ))
      ->setDisplayOptions('form', array(
        'type' => 'string_textfield',
        'weight' => -4,
      ))
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE)
      ->setRevisionable(TRUE);

    $fields['user_id'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('User'))
      ->setDescription(t('The Drupal user this contact is linked to.'))
      ->setRevisionable(TRUE)
      ->setSetting('target_type', 'user')
      ->setSetting('handler', 'default')
      ->setDefaultValueCallback('Drupal\node\Entity\Node::getCurrentUserId')
      ->setTranslatable(TRUE)
      ->setDisplayOptions('view', array(
        'label' => 'inline',
        'type' => 'entity_reference_label',
        'settings' => array(
          'link' => TRUE,
        ),
        'weight' => 0,
      ))
      ->setDisplayOptions('form', array(
        'type' => 'entity_reference_autocomplete',
        'weight' => 5,
        'settings' => array(
          'match_operator' => 'CONTAINS',
          'size' => '60',
          'autocomplete_type' => 'tags',
          'placeholder' => '',
        ),
      ))
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['status'] = BaseFieldDefinition::create('boolean')
      ->setLabel(t('Active'))
      ->setDescription(t('A boolean indicating whether the Contact is active.'))
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
      ->setDescription(t('The time that the contact was created.'))
      ->setRevisionable(TRUE);

    $fields['changed'] = BaseFieldDefinition::create('changed')
      ->setLabel(t('Changed'))
      ->setDescription(t('The time that the contact was last edited.'))
      ->setRevisionable(TRUE);

    return $fields;
  }

}
